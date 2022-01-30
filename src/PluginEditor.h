#pragma once
#include "PluginProcessor.h"
#include "SingleChannelIOWidget.h"
#include "SweepComponentEditor.h"
#include <customComponents/ReverseSlider.h>
#include <customComponents/SimpleLabel.h>
#include <customComponents/TitleBar.h>
#include <lookAndFeel/IEM_LaF.h>

typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

class MultiSweepAudioProcessorEditor
  : public AudioProcessorEditor
  , private Timer
{
public:
  MultiSweepAudioProcessorEditor(MultiSweepAudioProcessor&,
                                 AudioProcessorValueTreeState&);
  ~MultiSweepAudioProcessorEditor() override;

  void paint(Graphics&) override;
  void resized() override;
  void timerCallback() override;

private:
  void drawHeaderFooter(Rectangle<int>& canvas);
  void updateIOChannelCount();

private:
  static constexpr int editorWindowHeight = 600;
  static constexpr int editorWindowWidth = 800;
  static constexpr int maxOuputChannels = 10;

  LaF iemLookAndFeel;
  MultiSweepAudioProcessor& audioProcessor;
  AudioProcessorValueTreeState& valueTreeState;

  TitleBar<NoIOWidget, SingleChannelIOWidget<maxOuputChannels, true>> title;
  std::unique_ptr<ComboBoxAttachment> outputChannelsAttachment;

  Footer footer;

  SweepComponentEditor sweepEditor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessorEditor)
};
