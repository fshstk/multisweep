#pragma once
#include "FreqResponseDisplay.h"
#include "SweepComponentProcessor.h"
#include <IEM_LaF.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

class SweepComponentEditor : public juce::AudioProcessorEditor
{
public:
  SweepComponentEditor(SweepComponentProcessor& sweepProcessor,
                       juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&sweepProcessor)
    , valueTreeState(vts)
    , sweep(sweepProcessor)
    , prevChannelButton("previous channel", 0.5, lookAndFeel.ClText)
    , nextChannelButton("next channel", 0.0, lookAndFeel.ClText)
    , freqDisplay(sweepProcessor)
  {
    addAndMakeVisible(singleSweepButton);
    singleSweepButton.setButtonText("Sweep Selected Channel");
    singleSweepButton.onClick = [this] { sweep.startSweep({ .channel = 0 }); };

    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop Sweep");
    stopButton.onClick = [this] { sweep.stopSweep(); };

    addAndMakeVisible(clearButton);
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this] { sweep.clearData(); };

    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] { sweep.exportFilter(); };
    exportButton.setButtonText("Export");

    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Settings");

    addAndMakeVisible(freqDisplay);

    addAndMakeVisible(prevChannelButton);
    addAndMakeVisible(nextChannelButton);
  }

  void resized() override
  {
    auto area = getLocalBounds();

    auto bottomRow = area.removeFromBottom(150);
    auto firstButtonRow = bottomRow.removeFromTop(50);
    auto secondButtonRow = bottomRow.removeFromTop(50);
    auto thirdButtonRow = bottomRow;

    freqDisplay.setBounds(area);

    singleSweepButton.setBounds(firstButtonRow);

    stopButton.setBounds(
      secondButtonRow.removeFromLeft(secondButtonRow.getWidth() / 2));
    clearButton.setBounds(secondButtonRow);

    exportButton.setBounds(
      thirdButtonRow.removeFromLeft(thirdButtonRow.getWidth() / 2));
    settingsButton.setBounds(thirdButtonRow);
  }

private:
  juce::AudioProcessorValueTreeState& valueTreeState;
  LaF lookAndFeel;

  SweepComponentProcessor& sweep;

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
