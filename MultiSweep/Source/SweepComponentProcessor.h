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
  double lowerFreq = 20.0;
  double upperFreq = 20e3;
  double responseTailInSeconds = 1; // TODO: unused
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

    if (!isSweepActive())
      return;

    saveInputBuffer(buffer);

    jassert(audioSource);
    jassert(outputChannelMapper);
    outputChannelMapper->getNextAudioBlock(
      juce::AudioSourceChannelInfo(buffer));

    // We need to manually stop the source from looping: (Why...?!)
    const auto prevOutputBufferIndex = outputBufferIndex;
    outputBufferIndex = int(audioSource->getNextReadPosition());
    if (prevOutputBufferIndex > outputBufferIndex)
      stopSweep(); // TODO: shouldn't stop it here since we need the tail
  }

  void releaseResources() override
  {
    outputChannelMapper->releaseResources();
    audioSource->releaseResources();
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
    auto sweepBuffer = makeAudioBuffer(sweep.generateSignal());
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

    // This needs to happen AFTER all the memory stuff since everything runs
    // concurrently:
    sweepActive = true;
  }

  void stopSweep()
  {
    sweepActive = false;
    // processSweep(); // should this happen here? it will block the thread
  }

  bool isSweepActive() const { return sweepActive; }

  void exportFilter() const {}
  void clearData() {}

private:
  void saveInputBuffer(juce::AudioSampleBuffer& input)
  {
    jassert(inputBuffer);

    if (inputBufferIndex >= inputBuffer->getNumSamples()) {
      stopSweep();
      return;
    }

    const auto numSamples = input.getNumSamples();
    inputBuffer->copyFrom(0, inputBufferIndex, input, 0, 0, numSamples);
    inputBufferIndex += numSamples;
  }

  static const juce::AudioSampleBuffer makeAudioBuffer(
    const std::vector<float>& vector)
  {
    juce::AudioSampleBuffer buffer(1, int(vector.size()));
    for (size_t i = 0; i < vector.size(); ++i)
      buffer.setSample(0, int(i), vector[i]);
    return buffer;
  }

private:
  bool sweepActive = false;
  double fs = 0;
  int samplesPerBlock = 0;

  int outputBufferIndex = 0;
  int inputBufferIndex = 0;

  SweepComponentMetadata metadata;

  std::unique_ptr<juce::MemoryAudioSource> audioSource;
  std::unique_ptr<juce::ChannelRemappingAudioSource> outputChannelMapper;

  std::unique_ptr<juce::AudioSampleBuffer> inputBuffer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentProcessor)
};
