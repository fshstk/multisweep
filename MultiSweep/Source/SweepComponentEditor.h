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
#include "SweepComponentProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

/*

SweepComponent has 4 waveform displays:
1. raw recorded sweep response (live maybe?)
2. calculated IR
3. calculated freq response
4. calculated phase

separate class: audio thumbnail, takes in buffer/vector by reference,
                has a callback to redraw

*/

// Borrowed from the JUCE examples:
class RecordingThumbnail
  : public juce::Component
  , private juce::ChangeListener
{
public:
  RecordingThumbnail(SweepComponentProcessor& _sweepProcessor)
    : sweepProcessor(_sweepProcessor)
  {
    formatManager.registerBasicFormats();
    // thumbnail.addChangeListener(this);
    sweepProcessor.addThumbnailListener(this);
  }

  ~RecordingThumbnail() override { thumbnail.removeChangeListener(this); }

  void setThumbnail(const juce::AudioSampleBuffer* input, double sampleRate)
  {
    if (input) {
      thumbnail.reset(1, sampleRate, input->getNumSamples());
      thumbnail.addBlock(0, *input, 0, input->getNumSamples());
    }
    // TODO: need an else clause here to display something if input is nullptr
    repaint();
  }

  juce::AudioThumbnail& getAudioThumbnail() { return thumbnail; }

  void setDisplayFullThumbnail(bool displayFull)
  {
    displayFullThumb = displayFull;
    repaint();
  }

  void paint(juce::Graphics& g) override
  {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);

    if (thumbnail.getTotalLength() > 0.0) {
      auto endTime = displayFullThumb
                       ? thumbnail.getTotalLength()
                       : juce::jmax(30.0, thumbnail.getTotalLength());

      auto thumbArea = getLocalBounds();
      thumbnail.drawChannels(g, thumbArea.reduced(2), 0.0, endTime, 1.0f);
    } else {
      g.setFont(14.0f);
      g.drawFittedText("Press Start Sweep to record a sweep response.",
                       getLocalBounds(),
                       juce::Justification::centred,
                       2);
    }
  }

private:
  juce::AudioFormatManager formatManager;
  juce::AudioThumbnailCache thumbnailCache{ 10 };
  juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };

  SweepComponentProcessor& sweepProcessor;

  bool displayFullThumb = true;

  void changeListenerCallback(juce::ChangeBroadcaster* source) override
  {
    const auto fs = 44100.0; // TODO get this
    // if (source == &thumbnail)
    //   repaint();
    if (source == sweepProcessor.getThumbnailUpdateNotifier()) {
      const auto buffer = sweepProcessor.getInputBuffer();
      if (buffer)
        setThumbnail(buffer, fs);
      else
        setThumbnail({}, fs);
    }
  }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordingThumbnail)
};

//==============================================================================

class SweepComponentEditor : public juce::AudioProcessorEditor
{
public:
  SweepComponentEditor(SweepComponentProcessor& sweepProcessor)
    : AudioProcessorEditor(&sweepProcessor)
    , sweep(sweepProcessor)
    , sweepResponseThumbnail(sweepProcessor)
  {
    addAndMakeVisible(playButton);
    playButton.setButtonText("Start Sweep");
    playButton.onClick = [this] { sweep.startSweep(); };

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

    addAndMakeVisible(durationSlider);
    durationSlider.setSliderStyle(
      juce::Slider::SliderStyle::RotaryVerticalDrag);
    // Ignore the JUCE assertion failure, the slider doesn't like negative
    // angles but they're perfectly fine:
    durationSlider.setRotaryParameters(
      float(-5 / 6.0 * M_PI), float(+5 / 6.0 * M_PI), true);
    durationSlider.setRange(1.0, 20.0);
    durationSlider.setDoubleClickReturnValue(true, 2.0);
    durationSlider.setTextBoxStyle(
      juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    durationSlider.setNumDecimalPlacesToDisplay(1);
    // durationSlider.addListener();

    addAndMakeVisible(durationSliderLabel);
    durationSliderLabel.setText("Duration", juce::dontSendNotification);
    durationSliderLabel.setJustificationType(juce::Justification::centred);
    durationSliderLabel.attachToComponent(&durationSlider, false);

    addAndMakeVisible(sweepResponseThumbnail);
  }
  void resized() override
  {
    auto area = getLocalBounds();

    auto bottomRow = area.removeFromBottom(100);
    auto sliderArea = bottomRow.removeFromRight(100);
    auto firstButtonRow = bottomRow.removeFromTop(50);
    auto secondButtonRow = bottomRow;

    durationSlider.setBounds(sliderArea);
    sweepResponseThumbnail.setBounds(area.reduced(25));

    playButton.setBounds(
      firstButtonRow.removeFromLeft(firstButtonRow.getWidth() / 2));
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

  juce::Slider durationSlider;
  juce::Label durationSliderLabel;

  RecordingThumbnail sweepResponseThumbnail;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentEditor)
};
