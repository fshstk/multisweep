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
  RecordingThumbnail()
  {
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
  }

  ~RecordingThumbnail() override { thumbnail.removeChangeListener(this); }

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
      g.drawFittedText("(No file recorded)",
                       getLocalBounds(),
                       juce::Justification::centred,
                       2);
    }
  }

private:
  juce::AudioFormatManager formatManager;
  juce::AudioThumbnailCache thumbnailCache{ 10 };
  juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };

  bool displayFullThumb = false;

  void changeListenerCallback(juce::ChangeBroadcaster* source) override
  {
    if (source == &thumbnail)
      repaint();
  }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordingThumbnail)
};

//==============================================================================

class SweepComponentEditor : public juce::AudioProcessorEditor
{
public:
  SweepComponentEditor(SweepComponentProcessor* sweepProcessor)
    : AudioProcessorEditor(sweepProcessor)
  {}

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentEditor)
};
