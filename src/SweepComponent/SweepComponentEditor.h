#pragma once
#include "FreqResponseDisplay.h"
#include "SweepComponentProcessor.h"
#include <IEM_LaF.h>
#include <juce_audio_utils/juce_audio_utils.h>

class SweepComponentEditor : public juce::AudioProcessorEditor
{
public:
  SweepComponentEditor(SweepComponentProcessor& sweepProcessor,
                       juce::AudioProcessorValueTreeState& vts);

  void resized() override;

private:
  juce::AudioProcessorValueTreeState& valueTreeState;
  iem::LaF lookAndFeel;

  SweepComponentProcessor& parent;

  juce::TextButton singleSweepButton;
  juce::TextButton multipleSweepButton;

  juce::TextButton stopButton;
  juce::TextButton clearButton;

  juce::TextButton exportButton;
  juce::TextButton settingsButton;

  juce::ArrowButton prevChannelButton;
  juce::ArrowButton nextChannelButton;

  FreqResponseDisplay freqDisplay;
  std::vector<std::vector<float>> freqResponses;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentEditor)
};
