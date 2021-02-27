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
#include <lookAndFeel/IEM_LaF.h>

class SweepEditorContainer : public juce::TabbedComponent
{
public:
  SweepEditorContainer(
    std::vector<std::unique_ptr<SweepComponentProcessor>>& processors)
    : juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtBottom)
    , sweepProcessors(processors)
  {
    setLookAndFeel(&lookAndFeel); // probably doesn't do anything

    for (const auto& sweep : sweepProcessors)
      addTab(juce::String(sweep->getChannel()),
             lookAndFeel.ClBackground,
             new SweepComponentEditor(*sweep),
             true);
  }

private:
  LaF lookAndFeel;

  std::vector<std::unique_ptr<SweepComponentProcessor>>& sweepProcessors;
  std::vector<std::unique_ptr<SweepComponentEditor>> sweepEditors;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepEditorContainer)
};
