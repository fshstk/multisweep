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
#include "SweepComponentProcessor.h"
#include <AudioProcessorBase.h>
#define ProcessorClass MultiSweepAudioProcessor

constexpr auto numInputs = 1;
constexpr auto numOutputs = 64;

class MultiSweepAudioProcessor
  : public AudioProcessorBase<IOTypes::AudioChannels<numInputs>,
                              IOTypes::AudioChannels<numOutputs>>
{
public:
  MultiSweepAudioProcessor();
  ~MultiSweepAudioProcessor() override;

  // Functions for playing audio:
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

  bool hasEditor() const override { return true; }
  AudioProcessorEditor* createEditor() override;

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const String getProgramName(int) override { return {}; }
  void changeProgramName(int, const String&) override {}

  void getStateInformation(MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  void parameterChanged(const String& parameterID, float newValue) override;
  void updateBuffers() override; // use this to implement a buffer update method

public:
  constexpr static int numberOfInputChannels = 10;
  constexpr static int numberOfOutputChannels = 64;
  std::vector<std::unique_ptr<RangedAudioParameter>> createParameterLayout();

  SweepComponentProcessor sweep;

private:
  std::atomic<float>* outputChannelsSetting;
  // std::atomic<float>* param1;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessor)
};
