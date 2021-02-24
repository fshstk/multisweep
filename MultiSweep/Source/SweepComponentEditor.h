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
#include <lookAndFeel/IEM_LaF.h>

/*

SweepComponent has 4 waveform displays:
1. raw recorded sweep response (live maybe?)
2. calculated IR
3. calculated freq response
4. calculated phase

separate class: audio thumbnail, takes in buffer/vector by reference,
                has a callback to redraw

*/

class FreqResponseDisplay
  : public juce::Component
  , private juce::ChangeListener
{
public:
  FreqResponseDisplay(SweepComponentProcessor& processor)
    : sweep(processor)
  {
    sweep.addThumbnailListener(this);
  }
  ~FreqResponseDisplay() override { sweep.removeThumbnailListener(this); }

  void paint(juce::Graphics& g) override
  {

    const auto margin = 35;
    const auto area = getLocalBounds();

    const auto graph = area.reduced(margin);
    const auto xAxis = dft_log_bins(graph.getWidth(), 20.0, 20e3);

    const auto curve = sweep.getFrequencyResponse(graph.getWidth());

    auto yAxis = std::vector<float>(graph.getHeight());
    std::iota(yAxis.begin(), yAxis.end(), 0);
    std::transform(
      yAxis.cbegin(), yAxis.cend(), yAxis.begin(), [&yAxis](auto x) {
        const auto dB_low = -24;
        const auto dB_high = 24;
        return x * (dB_high - dB_low) / yAxis.size() + dB_low;
      });
    const auto yValues =
      std::vector<float>{ -24, -18, -12, -6, 0, 6, 12, 18, 24 };
    const auto findPixelForDb = [&yAxis](auto value) {
      return std::distance(
        yAxis.cbegin(), std::lower_bound(yAxis.cbegin(), yAxis.cend(), value));
    };
    auto yPixels = std::vector<int>(yValues.size());
    std::transform(
      yValues.cbegin(), yValues.cend(), yPixels.begin(), findPixelForDb);

    const auto values =
      std::vector<float>{ 20,  30,  40,  50,  60,  70,  80,   90,  100, 200,
                          300, 400, 500, 600, 700, 800, 900,  1e3, 2e3, 3e3,
                          4e3, 5e3, 6e3, 7e3, 8e3, 9e3, 10e3, 20e3 };
    const auto labeledValues = std::vector<float>{ 20, 200, 2e3, 20e3 };

    const auto findPixelForFrequency = [&xAxis](auto value) {
      return std::distance(
        xAxis.cbegin(), std::lower_bound(xAxis.cbegin(), xAxis.cend(), value));
    };

    auto xPixels = std::vector<int>(values.size());
    std::transform(
      values.cbegin(), values.cend(), xPixels.begin(), findPixelForFrequency);

    // g.setColour(juce::Colours::red);
    // g.drawRect(area);

    g.setColour(juce::Colours::darkslategrey);
    g.fillRect(graph);
    g.setColour(lookAndFeel.ClSeperator);
    // g.drawRect(graph);

    for (auto& pixel : xPixels) {
      const auto x = pixel + graph.getX();
      const auto y1 = graph.getY();
      const auto y2 = graph.getBottom();

      g.drawLine(x, y1, x, y2);
    }

    for (auto& pixel : yPixels) {
      const auto y = pixel + graph.getY();
      const auto x1 = graph.getX();
      const auto x2 = graph.getRight();

      g.drawLine(x1, y, x2, y);
    }

    for (auto& value : labeledValues) {
      const auto x = graph.getX() + findPixelForFrequency(value);
      const auto y = graph.getBottom() + margin / 2;

      const auto label = value < 1000 ? juce::String(value) + " Hz"
                                      : juce::String(value / 1000) + " kHz";

      g.setFont(lookAndFeel.robotoMedium);
      g.setColour(lookAndFeel.ClText);
      g.drawSingleLineText(label, int(x), int(y), juce::Justification::centred);
    }

    for (auto& value : yValues) {
      const auto x = graph.getX() - 5;
      // const auto y = graph.getBottom() - findPixelForDb(value);
      const auto y = graph.getBottom() - findPixelForDb(value) + 3;

      const auto label = juce::String(value) + " dB";

      g.setFont(lookAndFeel.robotoMedium);
      g.setColour(lookAndFeel.ClText);
      g.drawSingleLineText(label, int(x), int(y), juce::Justification::right);
    }

    const auto zeroDbCurve = std::vector<float>(xAxis.size(), 0);
    const auto plusTenDbCurve = std::vector<float>(xAxis.size(), 10);
    const auto minusTenDbCurve = std::vector<float>(xAxis.size(), -10);

    const auto drawPoint = [&](auto frequency, auto db) {
      const auto radius = 2;
      const auto x = graph.getX() + findPixelForFrequency(frequency);
      const auto y = graph.getBottom() - findPixelForDb(db);

      g.fillEllipse(x, y, radius, radius);
    };

    g.setColour(lookAndFeel.ClSeperator);
    g.drawRect(graph);

    g.setColour(juce::Colours::red);
    for (uint i = 0; i < curve.size(); ++i)
      drawPoint(xAxis[i], curve[i]);
  }

private:
  void changeListenerCallback(juce::ChangeBroadcaster*) override { repaint(); }

private:
  LaF lookAndFeel;
  SweepComponentProcessor& sweep;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreqResponseDisplay)
};

//==============================================================================

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

  ~RecordingThumbnail() override
  {
    sweepProcessor.removeThumbnailListener(this);
  }

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
    , freqResponse(sweepProcessor)
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

    addAndMakeVisible(freqResponse);
  }
  void resized() override
  {
    auto area = getLocalBounds();

    auto bottomRow = area.removeFromBottom(100);
    auto sliderArea = bottomRow.removeFromRight(100);
    auto firstButtonRow = bottomRow.removeFromTop(50);
    auto secondButtonRow = bottomRow;

    durationSlider.setBounds(sliderArea);
    freqResponse.setBounds(area);

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

  FreqResponseDisplay freqResponse;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentEditor)
};
