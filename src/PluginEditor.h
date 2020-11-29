#pragma once

#include "PluginProcessor.h"

class MultiSweepAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
  MultiSweepAudioProcessorEditor(MultiSweepAudioProcessor&);
  ~MultiSweepAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  MultiSweepAudioProcessor& audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessorEditor)
};
