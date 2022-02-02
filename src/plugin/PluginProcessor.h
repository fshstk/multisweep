#pragma once
#include "StereoPluginBase.h"
#include "SweepComponentProcessor.h"

class MultiSweepAudioProcessor : public StereoPluginBase
{
public:
  MultiSweepAudioProcessor();

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;

  SweepComponentProcessor sweep;

private:
  juce::AudioProcessorValueTreeState parameters;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessor)
};
