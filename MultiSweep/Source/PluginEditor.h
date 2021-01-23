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
#include "PluginProcessor.h"
#include <customComponents/ReverseSlider.h>
#include <customComponents/SimpleLabel.h>
#include <customComponents/TitleBar.h>
#include <lookAndFeel/IEM_LaF.h>

typedef ReverseSlider::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

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
  static constexpr int editorWindowHeight = 300;
  static constexpr int editorWindowWidth = 500;
  static constexpr int maxOuputChannels = 10;

  LaF iemLookAndFeel;
  MultiSweepAudioProcessor& audioProcessor;
  AudioProcessorValueTreeState& valueTreeState;

  TitleBar<NoIOWidget, AudioChannelsIOWidget<maxOuputChannels, true>> title;
  std::unique_ptr<ComboBoxAttachment> outputChannelsAttachment;

  Footer footer;

  // UI Elements:
  Slider demoSlider1;
  std::unique_ptr<SliderAttachment> slider1Attachment;
  ReverseSlider demoSlider2;
  std::unique_ptr<SliderAttachment> slider2Attachment;

  TextButton playButton;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessorEditor)
};

// Procedure for adding a slider/value:
// - add UI element editor member
// - add element attachment as editor member
// - add parameter as processor member
// - push back parameter in createParameterLayout()
// - attach vts parameter to processor member parameter
// - add listener to processor member parameter
// - addAndMakeVisible(UI element)
// - init attachment with connection b/w parameter and slider
// - ???
