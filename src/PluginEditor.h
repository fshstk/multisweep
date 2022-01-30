#pragma once
#include "PluginProcessor.h"
#include "SweepComponentEditor.h"

class MultiSweepAudioProcessorEditor
  : public AudioProcessorEditor
{
public:
  MultiSweepAudioProcessorEditor(MultiSweepAudioProcessor&,
                                 AudioProcessorValueTreeState&);

  void paint(Graphics&) override;
  void resized() override;

private:
  void drawHeaderFooter(Rectangle<int>& canvas);

private:
  static constexpr int editorWindowHeight = 600;
  static constexpr int editorWindowWidth = 800;

  MultiSweepAudioProcessor& audioProcessor;
  AudioProcessorValueTreeState& valueTreeState;

  SweepComponentEditor sweepEditor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessorEditor)
};
