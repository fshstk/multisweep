#include "SweepComponentProcessor.h"
#include "FFT.h"

void SweepComponentProcessor::prepareToPlay(double sampleRate,
                                            int maxSamplesPerBlock)
{
  fs = sampleRate;
  samplesPerBlock = maxSamplesPerBlock;
}

void SweepComponentProcessor::processBlock(juce::AudioSampleBuffer& buffer,
                                           juce::MidiBuffer&)
{
  jassert(fs > 0);
  if (fs <= 0)
    return;

  if (!sweepActive) {
    buffer.clear(); // mute input
    return;
  }

  saveInputBuffer(buffer);
  buffer.clear(); // don't leave anything in the buffer, it might get played

  jassert(audioSource);
  jassert(outputChannelMapper);

  if (sweepActive && !sweepFinished)
    outputChannelMapper->getNextAudioBlock(
      juce::AudioSourceChannelInfo(buffer));

  // We need to manually stop the source from looping: (Why...?!)
  const auto prevOutputBufferIndex = outputBufferIndex;
  outputBufferIndex = int(audioSource->getNextReadPosition());
  if (prevOutputBufferIndex > outputBufferIndex)
    sweepFinished = true;
}

void SweepComponentProcessor::releaseResources()
{
  if (audioSource)
    audioSource->releaseResources();
  if (outputChannelMapper)
    outputChannelMapper->releaseResources();
}

void SweepComponentProcessor::startSweep(SweepComponentMetadata metadata)
{
  jassert(fs > 0);
  jassert(samplesPerBlock > 0);

  // If sweep is already active, do nothing
  if (sweepActive)
    return;

  inputBufferIndex = 0;
  outputBufferIndex = 0;

  sweep.reset(new LogSweep(
    { fs, metadata.duration, { metadata.lowerFreq, metadata.upperFreq } }));
  auto sweepBuffer = makeBufferFromVector(sweep->generateSignal());
  audioSource.reset(new juce::MemoryAudioSource(sweepBuffer, true));

  // We need the outputChannelMapper so we can play the sweep on the desired
  // channel (otherwise it will just play on channel 0):
  outputChannelMapper.reset(
    new juce::ChannelRemappingAudioSource(audioSource.get(), false));
  outputChannelMapper->setOutputChannelMapping(0, metadata.channel);
  outputChannelMapper->prepareToPlay(samplesPerBlock, fs);

  const auto inputBufferSize =
    sweepBuffer.getNumSamples() + int(fs * metadata.responseTailInSeconds);
  inputBuffer.reset(new juce::AudioSampleBuffer(1, inputBufferSize));
  // inputBuffer is NOT guaranteed to be empty on initialization:
  inputBuffer->clear();

  // This needs to happen AFTER all the memory stuff since everything runs
  // concurrently:
  sweepActive = true;
  sweepFinished = false;
}

void SweepComponentProcessor::stopSweep()
{
  sweepActive = false;
  sweepFinished = false;
  sendChangeMessage();
}

void SweepComponentProcessor::exportFilter() const
{
  jassert(inputBuffer);
  jassert(sweep);
  if (inputBuffer && sweep) {
    const auto inputVector = makeVectorFromBuffer(*inputBuffer);
    const auto irVector = sweep->computeIR(inputVector);
    const auto freqResponse = dft_magnitude(irVector);
    const auto freqResponseDb = dft_magnitude_db(irVector);
    const auto freqBins = dft_lin_bins(float(fs), freqResponse.size() * 2);

    juce::FileChooser dialog(
      "Select a location to save the filter coefficients...");
    if (dialog.browseForFileToSave(true)) {
      juce::File file = dialog.getResult();
      auto fileContents = std::stringstream{};

      fileContents << "freq,mag,db\n";

      for (size_t i = 0; i < freqResponse.size(); ++i)
        fileContents << freqBins[i] << "," << freqResponse[i] << ","
                     << freqResponseDb[i] << "\n";

      file.replaceWithText(fileContents.str());
    }
  }
}

void SweepComponentProcessor::clearData()
{
  inputBuffer.reset();
  sendChangeMessage();
}

std::vector<float> SweepComponentProcessor::getImpulseResponse()
{
  jassert(inputBuffer);
  jassert(sweep);
  if (inputBuffer && sweep) {
    const auto inputVector = makeVectorFromBuffer(*inputBuffer);
    return sweep->computeIR(inputVector);
  }
  return {};
}

std::vector<float> SweepComponentProcessor::getFrequencyResponse(size_t numbins)
{
  jassert(inputBuffer);
  jassert(sweep);
  if (inputBuffer && sweep) {
    const auto inputVector = makeVectorFromBuffer(*inputBuffer);
    const auto irVector = sweep->computeIR(inputVector);
    return dft_magnitude_with_log_bins(irVector, float(fs), numbins);
  }
  return {};
}

void SweepComponentProcessor::saveInputBuffer(juce::AudioSampleBuffer& input)
{
  jassert(inputBuffer);

  if (inputBufferIndex >= inputBuffer->getNumSamples()) {
    stopSweep();
    return;
  }

  const auto numSamples = juce::jmin(
    inputBuffer->getNumSamples() - inputBufferIndex, input.getNumSamples());
  inputBuffer->copyFrom(0, inputBufferIndex, input, 0, 0, numSamples);
  inputBufferIndex += numSamples;
}
