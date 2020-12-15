/*
 ==============================================================================
 This file is part of the IEM plug-in suite.
 Author: Daniel Rudrich
 Copyright (c) 2017 - Institute of Electronic Music and Acoustics (IEM)
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

/*
 The following code taken from JUCE and modified.
 */

/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/
#include <JuceHeader.h>

namespace iem {

//==============================================================================
/**
    A component containing controls to let the user change the audio settings of
    an AudioDeviceManager object.

    Very easy to use - just create one of these and show it to the user.

    @see AudioDeviceManager

    @tags{Audio}
*/
class IEMAudioDeviceSelectorComponent
  : public Component
  , private ChangeListener
  , private Timer
{
public:
  //==============================================================================
  /** Creates the component.

      If your app needs only output channels, you might ask for a maximum of 0
     input channels, and the component won't display any options for choosing
     the input channels. And likewise if you're doing an input-only app.

      @param deviceManager            the device manager that this component
     should control
      @param minAudioInputChannels    the minimum number of audio input channels
     that the application needs
      @param maxAudioInputChannels    the maximum number of audio input channels
     that the application needs
      @param minAudioOutputChannels   the minimum number of audio output
     channels that the application needs
      @param maxAudioOutputChannels   the maximum number of audio output
     channels that the application needs
      @param showMidiInputOptions     if true, the component will allow the user
     to select which midi inputs are enabled
      @param showMidiOutputSelector   if true, the component will let the user
     choose a default midi output device
      @param showChannelsAsStereoPairs    if true, channels will be treated as
     pairs; if false, channels will be treated as a set of separate mono
     channels.
      @param hideAdvancedOptionsWithButton    if true, only the minimum amount
     of UI components are shown, with an "advanced" button that shows the rest
     of them
  */
  IEMAudioDeviceSelectorComponent(AudioDeviceManager& deviceManager,
                                  int minAudioInputChannels,
                                  int maxAudioInputChannels,
                                  int minAudioOutputChannels,
                                  int maxAudioOutputChannels,
                                  bool showMidiInputOptions,
                                  bool showMidiOutputSelector,
                                  bool showChannelsAsStereoPairs,
                                  bool hideAdvancedOptionsWithButton);

  /** Destructor */
  ~IEMAudioDeviceSelectorComponent() override;

  /** The device manager that this component is controlling */
  AudioDeviceManager& deviceManager;

  /** Sets the standard height used for items in the panel. */
  void setItemHeight(int itemHeight);

  /** Returns the standard height used for items in the panel. */
  int getItemHeight() const noexcept { return itemHeight; }

  /** Returns the ListBox that's being used to show the midi inputs, or nullptr
   * if there isn't one. */
  ListBox* getMidiInputSelectorListBox() const noexcept;

  //==============================================================================
  /** @internal */
  void resized() override;
  /** @internal */
  void timerCallback() override;

private:
  //==============================================================================
  void handleBluetoothButton();
  void updateDeviceType();
  void updateMidiOutput();
  void changeListenerCallback(ChangeBroadcaster*) override;
  void updateAllControls();

  std::unique_ptr<ComboBox> deviceTypeDropDown;
  std::unique_ptr<Label> deviceTypeDropDownLabel;
  std::unique_ptr<Component> audioDeviceSettingsComp;
  String audioDeviceSettingsCompType;
  int itemHeight;
  const int minOutputChannels, maxOutputChannels, minInputChannels,
    maxInputChannels;
  const bool showChannelsAsStereoPairs;
  const bool hideAdvancedOptionsWithButton;

  class MidiInputSelectorComponentListBox;
  std::unique_ptr<MidiInputSelectorComponentListBox> midiInputsList;
  std::unique_ptr<ComboBox> midiOutputSelector;
  std::unique_ptr<Label> midiInputsLabel, midiOutputLabel;
  std::unique_ptr<TextButton> bluetoothButton;

  bool jackActive = false;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IEMAudioDeviceSelectorComponent)
};

} // namespace juce
