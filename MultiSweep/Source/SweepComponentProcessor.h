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
#include <juce_dsp/juce_dsp.h>

/*

SweepProcessor does the following things:
0. stores settings about sweep length, duration, start/stop frequencies, out ch
1. generates & plays back a sine sweep
2. simultaneously records the response + fixed time or until signal below thresh
3. generates and stores IR, magnitude, phase, and frequency bins

separate class for recording?
separate class for playback?

*/
class SweepComponentProcessor : public juce::dsp::ProcessorBase
{
public:
  SweepComponentProcessor(int outputChannelToUse)
    : channel(outputChannelToUse)
  {}

  void prepare(const juce::dsp::ProcessSpec& audioSpec) override
  {
    fs = audioSpec.sampleRate;
    maxChannels = int(audioSpec.numChannels);
  }

  void process(
    const juce::dsp::ProcessContextReplacing<float>& audioContext) override
  {
    if (fs <= 0)
      return;

    if (channel < 0 || channel >= maxChannels)
      return;

    if (isSweepActive()) {
      // The order here is important since in/out might be the same block (?)
      addInputToBuffer(audioContext.getInputBlock());
      fillOutputWithSweep(audioContext.getOutputBlock());
    }
  }

  void reset() override
  {
    sweepActive = false;
    readIndex = 0;
    writeIndex = 0;

    sweepBuffer.reset();
    sweepObject.reset();
    responseBuffer.reset();
  }

  void startSweep()
  {
    sweepActive = true;

    sweepObject.reset(new LogSweep{ fs, duration, { lowerFreq, upperFreq } });
    sweepBuffer.reset(
      new AudioSampleBuffer{ makeAudioBuffer(sweepObject->generateSignal()) });

    const auto numResponseSamples =
      sweepBuffer->getNumSamples() + int(responseTailInSeconds * fs);

    responseBuffer.reset(new AudioSampleBuffer{ 1, numResponseSamples });
  }

  bool isSweepActive() { return sweepActive; }

private:
  // TODO: This should be AudioBlock<float> (?)
  template<typename SampleType>
  void addInputToBuffer(const juce::dsp::AudioBlock<SampleType>& input)
  {
    // save block to first empty region in responseBuffer,
    // then increment the write pointer. when buffer is full, stop the sweep.
    // also update thumbnail here?
  }

  template<typename SampleType>
  void fillOutputWithSweep(juce::dsp::AudioBlock<SampleType>& output)
  {
    const auto remainingBuffer = sweepBuffer->getNumSamples() - writeIndex;
    const auto blockSize = output.getNumSamples();

    if (remainingBuffer <= 0)
      return;

    if (remainingBuffer >= int(blockSize)) {
      // write sweepBuffer[writePointer:writePointer+blockSize-1] -> block
    }

    else {
      // write sweepBuffer[writePointer:writePointer+remainingBuffer]
      //    -> block[0:remainingBuffer]
      // write zeros -> block[remainingBuffer+1 : blockSize-1]
    }

    writeIndex += blockSize;
  }

  const juce::AudioSampleBuffer makeAudioBuffer(
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
  int maxChannels = 0;
  int channel = -1;

  int readIndex = 0;
  int writeIndex = 0;

  static constexpr auto lowerFreq = 20.0;
  static constexpr auto upperFreq = 20e3;
  static constexpr auto duration = 2;
  static constexpr auto responseTailInSeconds = 1;

  std::unique_ptr<LogSweep> sweepObject;
  std::unique_ptr<AudioSampleBuffer> responseBuffer; // TODO: float->SampleType
  std::unique_ptr<AudioSampleBuffer> sweepBuffer;    // TODO: AudioBuffer
};

