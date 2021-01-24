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
class SweepComponentProcessor : public juce::AudioProcessor
{
public:
  SweepComponentProcessor(int outputChannelToUse)
    : channel(outputChannelToUse)
  {}

  const juce::String getName() const override { return {}; }
  double getTailLengthSeconds() const override { return 0; }
  bool acceptsMidi() const override { return 0; }
  bool producesMidi() const override { return 0; }
  juce::AudioProcessorEditor* createEditor() override {}
  bool hasEditor() const override { return 0; }
  int getNumPrograms() override { return 0; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return {}; }
  void changeProgramName(int, const juce::String&) override {}
  void getStateInformation(juce::MemoryBlock&) override {}
  void setStateInformation(const void*, int) override {}

  void prepareToPlay(double sampleRate, int maxSamplesPerBlock) override
  {
    fs = sampleRate;
    samplesPerBlock = maxSamplesPerBlock;
  }

  void processBlock(juce::AudioBuffer<float>& buffer,
                    juce::MidiBuffer&) override
  {
    if (fs <= 0)
      return;

    if (channel < 0 || channel >= buffer.getNumChannels())
      return;

    if (isSweepActive()) {
      // saveInputBuffer(buffer);

      jassert(audioSource);
      audioSource->getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));

      const auto prevWriteIndex = writeIndex;
      writeIndex = int(audioSource->getNextReadPosition());

      // We need to manually stop the source from looping. Why...?!
      if (prevWriteIndex > writeIndex)
        stopSweep();
    }
  }

  void releaseResources() override { audioSource.reset(); }

  void startSweep()
  {
    jassert(fs > 0 && samplesPerBlock > 0);

    const auto sweep = LogSweep(fs, duration, { lowerFreq, upperFreq });
    auto buffer = makeAudioBuffer(sweep.generateSignal());

    audioSource.reset(new juce::MemoryAudioSource(buffer, true));
    audioSource->prepareToPlay(samplesPerBlock, fs);

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
  int channel = -1;

  int writeIndex = 0;

  static constexpr auto lowerFreq = 20.0;
  static constexpr auto upperFreq = 20e3;
  static constexpr auto duration = 2;
  static constexpr auto responseTailInSeconds = 1;

  std::unique_ptr<juce::MemoryAudioSource> audioSource;
};
