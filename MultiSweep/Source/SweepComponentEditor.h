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
#include "FreqResponseDisplay.h"
#include "SweepComponentProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <lookAndFeel/IEM_LaF.h>

class SweepComponentEditor : public juce::AudioProcessorEditor
{
public:
  SweepComponentEditor(SweepComponentProcessor& sweepProcessor)
    : AudioProcessorEditor(&sweepProcessor)
    , sweep(sweepProcessor)
    , prevChannelButton("previous channel", 0.5, lookAndFeel.ClText)
    , nextChannelButton("next channel", 0.0, lookAndFeel.ClText)
    , freqDisplay(sweepProcessor)
  {
    addAndMakeVisible(singleSweepButton);
    singleSweepButton.setButtonText("Sweep Selected Channel");
    singleSweepButton.onClick = [this] {
      sweep.startSweep({ .channel = selectedChannel });
    };

    addAndMakeVisible(multipleSweepButton);
    multipleSweepButton.setButtonText("Sweep All Channels");

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

    const auto arrowButtonWidth = 20;
    const auto arrowButtonYMargin = 50;
    const auto arrowButtonInsideMargin = 20;
    prevChannelButton.setBounds(area.removeFromLeft(arrowButtonWidth)
                                  .withTrimmedBottom(arrowButtonYMargin)
                                  .withTrimmedTop(arrowButtonYMargin));
    nextChannelButton.setBounds(area.removeFromRight(arrowButtonWidth)
                                  .withTrimmedBottom(arrowButtonYMargin)
                                  .withTrimmedTop(arrowButtonYMargin));

    freqDisplay.setBounds(area.withTrimmedLeft(arrowButtonInsideMargin));

    singleSweepButton.setBounds(
      firstButtonRow.removeFromLeft(firstButtonRow.getWidth() / 2));
    multipleSweepButton.setBounds(firstButtonRow);

    stopButton.setBounds(
      secondButtonRow.removeFromLeft(secondButtonRow.getWidth() / 2));
    clearButton.setBounds(secondButtonRow);

    exportButton.setBounds(
      thirdButtonRow.removeFromLeft(thirdButtonRow.getWidth() / 2));
    settingsButton.setBounds(thirdButtonRow);
  }

private:
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
