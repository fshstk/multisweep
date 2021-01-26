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
  , footer()
  , sweepEditor(p.sweep)
{
  setSize(editorWindowWidth, editorWindowHeight);
  setLookAndFeel(&iemLookAndFeel);

  addAndMakeVisible(title);
  addAndMakeVisible(footer);

  title.setTitle(String("Multi"), String("Sweep"));
  title.setFont(iemLookAndFeel.robotoBold, iemLookAndFeel.robotoLight);

  outputChannelsAttachment = std::make_unique<ComboBoxAttachment>(
    valueTreeState,
    "outputChannelsSetting",
    *title.getOutputWidgetPtr()->getChannelsCbPointer());

  addAndMakeVisible(sweepEditor);

  startTimer(20); // --> timerCallback()
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
  Rectangle<int> area = getLocalBounds();
  drawHeaderFooter(area);
  sweepEditor.setBounds(area);
}

void MultiSweepAudioProcessorEditor::timerCallback()
{
  updateIOChannelCount();
}

void MultiSweepAudioProcessorEditor::updateIOChannelCount()
{
  title.setMaxSize(audioProcessor.getMaxSize());
}

void MultiSweepAudioProcessorEditor::drawHeaderFooter(Rectangle<int>& canvas)
{
  const int leftRightMargin = 30;
  const int headerHeight = 60;
  const int footerHeight = 25;

  Rectangle<int> footerArea(canvas.removeFromBottom(footerHeight));
  footer.setBounds(footerArea);

  canvas.removeFromLeft(leftRightMargin);
  canvas.removeFromRight(leftRightMargin);
  Rectangle<int> headerArea = canvas.removeFromTop(headerHeight);
  title.setBounds(headerArea);
  canvas.removeFromTop(10);
  canvas.removeFromBottom(5);
}
