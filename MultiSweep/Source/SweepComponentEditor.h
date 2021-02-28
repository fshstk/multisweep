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
    , freqDisplay(sweepProcessor)
  {
    addAndMakeVisible(playButton);
    playButton.setButtonText("Start Sweep");
    playButton.onClick = [this] {
      sweep.startSweep({ .channel = channelSelector.getSelectedItemIndex() });
    };

    // TODO: clear & stop could be the same button
    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop Sweep");
    stopButton.onClick = [this] { sweep.stopSweep(); };

    addAndMakeVisible(clearButton);
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this] { sweep.clearData(); };

    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] { sweep.exportFilter(); };
    exportButton.setButtonText("Export");

    addAndMakeVisible(freqDisplay);

    // TODO: would be nice to override AudioChannelsIOWidget for this purpose
    addAndMakeVisible(channelSelector);
    channelSelector.addItemList({ "1", "2" }, 1); // TODO: all channels
    channelSelector.addSeparator();
    channelSelector.addItem("all", 100);
  }
  void resized() override
  {
    auto area = getLocalBounds();

    auto bottomRow = area.removeFromBottom(100);
    auto firstButtonRow = bottomRow.removeFromTop(50);
    auto secondButtonRow = bottomRow;

    freqDisplay.setBounds(area);

    auto playButtonArea =
      firstButtonRow.removeFromLeft(firstButtonRow.getWidth() / 2);
    channelSelector.setBounds(
      playButtonArea.removeFromRight(playButtonArea.getWidth() / 2));
    playButton.setBounds(playButtonArea);

    stopButton.setBounds(firstButtonRow);
    exportButton.setBounds(
      secondButtonRow.removeFromLeft(secondButtonRow.getWidth() / 2));
    clearButton.setBounds(secondButtonRow);
  }

private:
  SweepComponentProcessor& sweep;

  juce::TextButton playButton;
  juce::TextButton stopButton;
  juce::TextButton clearButton;
  juce::TextButton exportButton;

  juce::ComboBox channelSelector;

  FreqResponseDisplay freqDisplay;
  std::vector<std::vector<float>> freqResponses;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentEditor)
};
