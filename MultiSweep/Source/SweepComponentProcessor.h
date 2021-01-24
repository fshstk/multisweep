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
#include "ImpulseResponse.h"
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

  void processBlock(juce::AudioBuffer<float>& buffer,
                    juce::MidiBuffer&) override
  {
    jassert(fs > 0);
    jassert(metadata.channel >= 0);
    jassert(metadata.channel < buffer.getNumChannels());
    if (fs <= 0)
      return;

    if (metadata.channel < 0 || metadata.channel >= buffer.getNumChannels())
      return;

    if (isSweepActive()) {
      // saveInputBuffer(buffer);

      jassert(audioSource);
      jassert(outputChannelMapper);
      outputChannelMapper->getNextAudioBlock(
        juce::AudioSourceChannelInfo(buffer));

      // We need to manually stop the source from looping: (Why...?!)
      const auto prevWriteIndex = writeIndex;
      writeIndex = int(audioSource->getNextReadPosition());
      if (prevWriteIndex > writeIndex)
        stopSweep();
    }
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

    const auto sweep = LogSweep(
      fs, metadata.duration, { metadata.lowerFreq, metadata.upperFreq });
    auto buffer = makeAudioBuffer(sweep.generateSignal());
    audioSource.reset(new juce::MemoryAudioSource(buffer, true));

    // We need the outputChannelMapper so we can play the sweep on the desired
    // channel (otherwise it will just play on channel 0):
    outputChannelMapper.reset(
      new juce::ChannelRemappingAudioSource(audioSource.get(), false));
    outputChannelMapper->setOutputChannelMapping(0, metadata.channel);
    outputChannelMapper->prepareToPlay(samplesPerBlock, fs);

    // This needs to happen AFTER all the memory stuff since everything runs
    // concurrently:
    sweepActive = true;
  }

  void stopSweep()
  {
    sweepActive = false;
    writeIndex = 0;
    // audioSource.reset();
    // processSweep();
  }

  bool isSweepActive() const { return sweepActive; }

private:
  void saveInputBuffer(juce::AudioSampleBuffer& input)
  {
    // save block to first empty region in responseBuffer,
    // then increment the write pointer. when buffer is full, stop the sweep.
    // also update thumbnail here?
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
  int writeIndex = 0;

  SweepComponentMetadata metadata;

  std::unique_ptr<juce::MemoryAudioSource> audioSource;
  std::unique_ptr<juce::ChannelRemappingAudioSource> outputChannelMapper;
};
