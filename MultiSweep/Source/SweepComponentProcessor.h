#pragma once
#include "LogSweep.h"
#include "fft.h"
#include <juce_audio_processors/juce_audio_processors.h>

struct SweepComponentMetadata
{
  int channel = 0;
  double duration = 2;
  double lowerFreq = 10.0;
  double upperFreq = 22e3;
  double responseTailInSeconds = 1;
};

class SweepComponentProcessor
  : public juce::AudioProcessor
  , public juce::ChangeBroadcaster
{
public:
  SweepComponentProcessor() = default;

  const juce::String getName() const override { return {}; }
  double getTailLengthSeconds() const override { return 0; }

  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }

  int getNumPrograms() override { return 0; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  void changeProgramName(int, const juce::String&) override {}
  const juce::String getProgramName(int) override { return {}; }

  void getStateInformation(juce::MemoryBlock&) override {}
  void setStateInformation(const void*, int) override {}

  bool hasEditor() const override { return true; }
  juce::AudioProcessorEditor* createEditor() override { return nullptr; }

  void prepareToPlay(double sampleRate, int maxSamplesPerBlock) override
  {
    fs = sampleRate;
    samplesPerBlock = maxSamplesPerBlock;
  }

  void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
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

  void releaseResources() override
  {
    if (audioSource)
      audioSource->releaseResources();
    if (outputChannelMapper)
      outputChannelMapper->releaseResources();
  }

  void startSweep(SweepComponentMetadata metadata)
  {
    jassert(fs > 0);
    jassert(samplesPerBlock > 0);

    // If sweep is already active, do nothing
    if (sweepActive)
      return;

    inputBufferIndex = 0;
    outputBufferIndex = 0;

    sweep.reset(new LogSweep(
      fs, metadata.duration, { metadata.lowerFreq, metadata.upperFreq }));
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

  void stopSweep()
  {
    sweepActive = false;
    sweepFinished = false;
    sendChangeMessage();
  }

  void exportFilter() const
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

  void clearData()
  {
    inputBuffer.reset();
    sendChangeMessage();
  }

  std::vector<float> getImpulseResponse()
  {
    jassert(inputBuffer);
    jassert(sweep);
    if (inputBuffer && sweep) {
      const auto inputVector = makeVectorFromBuffer(*inputBuffer);
      return sweep->computeIR(inputVector);
    }
    return {};
  }

  std::vector<float> getFrequencyResponse(uint numbins)
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

private:
  void saveInputBuffer(juce::AudioSampleBuffer& input)
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

  template<typename T>
  static juce::AudioBuffer<T> makeBufferFromVector(const std::vector<T>& vector)
  {
    juce::AudioBuffer<T> buffer(1, int(vector.size()));
    for (size_t i = 0; i < vector.size(); ++i)
      buffer.setSample(0, int(i), vector[i]);
    return buffer;
  }

  template<typename T>
  static std::vector<T> makeVectorFromBuffer(const juce::AudioBuffer<T>& buffer)
  {
    const float* const ptr = buffer.getReadPointer(0);
    const auto length = buffer.getNumSamples();
    return std::vector<T>(ptr, ptr + length);
  }

private:
  bool sweepActive = false;   // used in check for recording AND playback
  bool sweepFinished = false; // only used in check for playback
  double fs = 0;
  int samplesPerBlock = 0;

  int outputBufferIndex = 0;
  int inputBufferIndex = 0;

  std::unique_ptr<juce::MemoryAudioSource> audioSource;
  std::unique_ptr<juce::ChannelRemappingAudioSource> outputChannelMapper;

  std::unique_ptr<juce::AudioSampleBuffer> inputBuffer;
  std::unique_ptr<ImpulseResponse> sweep;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentProcessor)
};
