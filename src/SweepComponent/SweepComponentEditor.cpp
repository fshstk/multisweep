#include "SweepComponentEditor.h"

SweepComponentEditor::SweepComponentEditor(
  SweepComponentProcessor& sweepProcessor,
  juce::AudioProcessorValueTreeState& vts)
  : AudioProcessorEditor(&sweepProcessor)
  , valueTreeState(vts)
  , parent(sweepProcessor)
  , prevChannelButton("previous channel", 0.5, lookAndFeel.ClText)
  , nextChannelButton("next channel", 0.0, lookAndFeel.ClText)
  , freqDisplay(sweepProcessor)
{
  addAndMakeVisible(singleSweepButton);
  singleSweepButton.setButtonText("Sweep Selected Channel");
  singleSweepButton.onClick = [this] { parent.startSweep({ .channel = 0 }); };

  addAndMakeVisible(stopButton);
  stopButton.setButtonText("Stop Sweep");
  stopButton.onClick = [this] { parent.stopSweep(); };

  addAndMakeVisible(clearButton);
  clearButton.setButtonText("Clear");
  clearButton.onClick = [this] { parent.clearData(); };

  addAndMakeVisible(exportButton);
  exportButton.onClick = [this] { parent.exportFilter(); };
  exportButton.setButtonText("Export");

  addAndMakeVisible(settingsButton);
  settingsButton.setButtonText("Settings");

  addAndMakeVisible(freqDisplay);

  addAndMakeVisible(prevChannelButton);
  addAndMakeVisible(nextChannelButton);
}

void SweepComponentEditor::resized()
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
