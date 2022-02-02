#pragma once
#include "PluginProcessor.h"
#include "SweepComponentEditor.h"

class MultiSweepAudioProcessorEditor
  : public juce::AudioProcessorEditor
{
public:
  MultiSweepAudioProcessorEditor(MultiSweepAudioProcessor&,
                                 juce::AudioProcessorValueTreeState&);

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  void drawHeaderFooter(juce::Rectangle<int>& canvas);

private:
  static constexpr int editorWindowHeight = 600;
  static constexpr int editorWindowWidth = 800;

  MultiSweepAudioProcessor& audioProcessor;
  juce::AudioProcessorValueTreeState& valueTreeState;

  SweepComponentEditor sweepEditor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessorEditor)
};
