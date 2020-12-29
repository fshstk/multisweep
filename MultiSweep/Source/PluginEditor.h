/*
 ==============================================================================
 This file is part of the IEM plug-in suite.
 Author: Fabian Hummel, David Neussl
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

// Plugin Design Essentials
#include <customComponents/TitleBar.h>
#include <lookAndFeel/IEM_LaF.h>

// Custom Components
#include <customComponents/ReverseSlider.h>
#include <customComponents/SimpleLabel.h>

typedef ReverseSlider::SliderAttachment
  SliderAttachment; // all ReverseSliders will make use of the parameters'
                    // valueToText() function
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
/**
 */
class PluginTemplateAudioProcessorEditor
  : public AudioProcessorEditor
  , private Timer
{
public:
  PluginTemplateAudioProcessorEditor(PluginTemplateAudioProcessor&,
                                     AudioProcessorValueTreeState&);
  ~PluginTemplateAudioProcessorEditor() override;

  //==============================================================================
  void paint(Graphics&) override;
  void resized() override;

  void timerCallback() override;

private:
  // ====================== begin essentials ==================
  // lookAndFeel class with the IEM plug-in suite design
  LaF globalLaF;

  // stored references to the AudioProcessor and ValueTreeState holding all the
  // parameters
  PluginTemplateAudioProcessor& audioProcessor;
  AudioProcessorValueTreeState& valueTreeState;

  /* title and footer component
   title component can hold different widgets for in- and output:
      - NoIOWidget (if there's no need for an input or output widget)
      - AudioChannelsIOWidget<maxNumberOfChannels, isChoosable>
      - AmbisonicIOWidget<maxOrder>
      - DirectivitiyIOWidget
   */
  TitleBar<AudioChannelsIOWidget<10, true>, AmbisonicIOWidget<>> title;
  OSCFooter footer;
  // =============== end essentials ============

  // Attachments to create a connection between IOWidgets comboboxes
  // and the associated parameters
  std::unique_ptr<ComboBoxAttachment> cbInputChannelsSettingAttachment;
  std::unique_ptr<ComboBoxAttachment> cbOrderSettingAttachment;
  std::unique_ptr<ComboBoxAttachment> cbNormalizationSettingAttachment;

  // Demo stuff
  Slider slParam1;
  ReverseSlider slParam2;
  std::unique_ptr<SliderAttachment> slParam1Attachment, slParam2Attachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
    PluginTemplateAudioProcessorEditor)
};
