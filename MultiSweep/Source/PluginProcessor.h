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

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessor)
};
