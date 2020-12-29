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

#include "PluginEditor.h"
#include "PluginProcessor.h"

MultiSweepAudioProcessorEditor::MultiSweepAudioProcessorEditor(
  MultiSweepAudioProcessor& p,
  AudioProcessorValueTreeState& vts)
  : AudioProcessorEditor(&p)
  , audioProcessor(p)
  , valueTreeState(vts)
  , footer(p.getOSCParameterInterface())
{
  setSize(editorWindowWidth, editorWindowHeight);
  setLookAndFeel(&iemLookAndFeel);

  addAndMakeVisible(&title);
  title.setTitle(String("Multi"), String("Sweep"));
  title.setFont(iemLookAndFeel.robotoBold, iemLookAndFeel.robotoLight);
  addAndMakeVisible(&footer);

  cbInputChannelsSettingAttachment.reset(
    new ComboBoxAttachment(valueTreeState,
                           "inputChannelsSetting",
                           *title.getInputWidgetPtr()->getChannelsCbPointer()));

  addAndMakeVisible(slParam1);
  slParam1Attachment.reset(
    new SliderAttachment(valueTreeState, "param1", slParam1));
  addAndMakeVisible(slParam2);
  slParam2Attachment.reset(
    new SliderAttachment(valueTreeState, "param2", slParam2));

  startTimer(20);
}

MultiSweepAudioProcessorEditor::~MultiSweepAudioProcessorEditor()
{
  setLookAndFeel(nullptr);
}

void MultiSweepAudioProcessorEditor::paint(Graphics& g)
{
  g.fillAll(iemLookAndFeel.ClBackground);
}

void MultiSweepAudioProcessorEditor::resized()
{
  // ============ BEGIN: header and footer ============
  const int leftRightMargin = 30;
  const int headerHeight = 60;
  const int footerHeight = 25;
  Rectangle<int> area(getLocalBounds());

  Rectangle<int> footerArea(area.removeFromBottom(footerHeight));
  footer.setBounds(footerArea);

  area.removeFromLeft(leftRightMargin);
  area.removeFromRight(leftRightMargin);
  Rectangle<int> headerArea = area.removeFromTop(headerHeight);
  title.setBounds(headerArea);
  area.removeFromTop(10);
  area.removeFromBottom(5);
  // =========== END: header and footer =================

  // try to not use explicit coordinates to position your GUI components
  // the removeFrom...() methods are quite handy to create scalable areas
  // best practice would be the use of flexBoxes...
  // the following is medium level practice ;-)
  Rectangle<int> sliderRow = area.removeFromTop(50);
  slParam1.setBounds(sliderRow.removeFromLeft(150));
  slParam2.setBounds(sliderRow.removeFromRight(150));
}

void MultiSweepAudioProcessorEditor::timerCallback()
{
  // Update titleBar widgets according to available input/output channel counts:
  title.setMaxSize(audioProcessor.getMaxSize());
}
