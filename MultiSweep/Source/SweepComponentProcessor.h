/*
 ==============================================================================
 This file is part of the IEM plug-in suite.
 Authors: Fabian Hummel, David Neussl
 Copyright (c) 2020 - Institute of Electronic Music and Acoustics (IEM)
 https://iem.at

 The IEM plug-in suite is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 The IEM plug-in suite is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this software.  If not, see <https://www.gnu.org/licenses/>.
 ==============================================================================
 */

#pragma once
#include "LogSweep.h"
#include "fft.h"
#include <juce_audio_processors/juce_audio_processors.h>

/*

SweepProcessor does the following things:
0. stores settings about sweep length, duration, start/stop frequencies, out ch
1. generates & plays back a sine sweep
2. simultaneously records the response + fixed time or until signal below thresh
3. generates and stores IR, magnitude, phase, and frequency bins

separate class for recording?
separate class for playback?

*/

struct SweepComponentMetadata
{
  int channel = 0;
  double duration = 2;
  double lowerFreq = 10.0;
  double upperFreq = 22e3;
  double responseTailInSeconds = 1;
};

class SweepComponentProcessor : public juce::AudioProcessor
{
public:
  SweepComponentProcessor(SweepComponentMetadata metadata_)
    : metadata(metadata_)
  {}

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
  juce::AudioProcessorEditor* createEditor() override;

  void prepareToPlay(double sampleRate, int maxSamplesPerBlock) override
  {
    fs = sampleRate;
    samplesPerBlock = maxSamplesPerBlock;
  }

  void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
  {
    jassert(fs > 0);
    jassert(metadata.channel >= 0);
    jassert(metadata.channel < buffer.getNumChannels());
    if (fs <= 0)
      return;

    if (metadata.channel < 0 || metadata.channel >= buffer.getNumChannels())
      return;

    if (!isSweepActive()) {
      // TODO: we can't mute input because of multiple sweep objects!
      // buffer.clear();
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

  void startSweep()
  {
    jassert(fs > 0);
    jassert(samplesPerBlock > 0);

    // If sweep is already active, should we do nothing or stop/restart?
    if (isSweepActive())
      return;

    inputBufferIndex = 0;
    outputBufferIndex = 0;

    const auto sweep = LogSweep(
      fs, metadata.duration, { metadata.lowerFreq, metadata.upperFreq });
    auto sweepBuffer = makeBufferFromVector(sweep.generateSignal());
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
    // TODO: two separate methods depending on if sweep is finished or cancelled
    sweepActive = false;
    sweepFinished = false;

    // NOTE: *DON'T* do this on the audio thread!!!
    irBuffer.reset(new juce::AudioSampleBuffer(getImpulseResponse()));

    thumbnailUpdateNotifier.sendChangeMessage();
    // processSweep(); // should this happen here? it will block the thread
  }

  bool isSweepActive() const { return sweepActive; }

  const juce::AudioSampleBuffer* getInputBuffer()
  {
    // NOTE: this can be nullptr!
    // return inputBuffer.get();
    return irBuffer.get();
  }

  void exportFilter() const
  {
    if (inputBuffer) {
      const auto sweep = LogSweep(
        fs, metadata.duration, { metadata.lowerFreq, metadata.upperFreq });
      const auto inputVector = makeVectorFromBuffer(*inputBuffer);
      const auto irVector = sweep.computeIR(inputVector);
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
    thumbnailUpdateNotifier.sendChangeMessage();
  }

  juce::AudioSampleBuffer getImpulseResponse()
  {
    // TODO: everyhwere sweep gets created, use a member pointer (to base
    // class!) instead
    const auto sweep = LogSweep(
      fs, metadata.duration, { metadata.lowerFreq, metadata.upperFreq });
    if (inputBuffer) {
      const auto inputVector = makeVectorFromBuffer(*inputBuffer);
      const auto irVector = sweep.computeIR(inputVector);
      irBuffer.reset(
        new juce::AudioSampleBuffer(makeBufferFromVector(irVector)));
      return *irBuffer;
    } else {
      return juce::AudioSampleBuffer();
    }
  }

  std::vector<float> getFrequencyResponse(uint numbins)
  {
    // TODO: everyhwere sweep gets created, use a member pointer (to base
    // class!) instead
    if (inputBuffer) {
      const auto sweep = LogSweep(
        fs, metadata.duration, { metadata.lowerFreq, metadata.upperFreq });
      const auto inputVector = makeVectorFromBuffer(*inputBuffer);
      const auto irVector = sweep.computeIR(inputVector);
      const auto freqResponse =
        dft_magnitude_with_log_bins(irVector, float(fs), numbins);
      return freqResponse;
    }
    return {};
  }

  // TODO: Probably should rename this to SweepListener or some such...
  // Not only the thumbnail but also the sweep processing gets done via this.
  void addThumbnailListener(juce::ChangeListener* listener)
  {
    thumbnailUpdateNotifier.addChangeListener(listener);
  }

  void removeThumbnailListener(juce::ChangeListener* listener)
  {
    thumbnailUpdateNotifier.removeChangeListener(listener);
  }

  juce::ChangeBroadcaster* getThumbnailUpdateNotifier()
  {
    return &thumbnailUpdateNotifier;
  }

  int getChannel() { return metadata.channel; }

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

  // TODO: These should probably even be non-member functions:
  template<typename T>
  static juce::AudioBuffer<T> makeBufferFromVector(const std::vector<T>& vector)
  {
    juce::AudioBuffer<T> buffer(1, int(vector.size()));
    for (size_t i = 0; i < vector.size(); ++i)
      buffer.setSample(0, int(i), vector[i]);
    return buffer;
  }

  // TODO: These should probably even be non-member functions:
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

  SweepComponentMetadata metadata;

  std::unique_ptr<juce::MemoryAudioSource> audioSource;
  std::unique_ptr<juce::ChannelRemappingAudioSource> outputChannelMapper;

  std::unique_ptr<juce::AudioSampleBuffer> inputBuffer;
  std::unique_ptr<juce::AudioSampleBuffer> irBuffer;

  juce::ChangeBroadcaster thumbnailUpdateNotifier;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentProcessor)
};
