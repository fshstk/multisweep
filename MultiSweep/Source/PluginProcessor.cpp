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

#include "PluginProcessor.h"
#include "PluginEditor.h"

MultiSweepAudioProcessor::MultiSweepAudioProcessor()
  : AudioProcessorBase(
      BusesProperties()
        .withInput("Input", AudioChannelSet::discreteChannels(10), true)
        .withOutput("Output", AudioChannelSet::discreteChannels(64), true),
      createParameterLayout())
{
  outputChannelsSetting =
    parameters.getRawParameterValue("outputChannelsSetting");
  param1 = parameters.getRawParameterValue("param1");
  param2 = parameters.getRawParameterValue("param2");

  parameters.addParameterListener("outputChannelsSetting", this);
  parameters.addParameterListener("param1", this);
  parameters.addParameterListener("param2", this);
}

MultiSweepAudioProcessor::~MultiSweepAudioProcessor() {}

void MultiSweepAudioProcessor::prepareToPlay(double sampleRate,
                                             int samplesPerBlock)
{
  // TODO: magic number
  checkInputAndOutput(this, 1, static_cast<int>(*outputChannelsSetting), true);
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  ignoreUnused(sampleRate, samplesPerBlock);

  sweep.prepareToPlay(samplesPerBlock, sampleRate);
}

void MultiSweepAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.

  sweep.releaseResources();
}

void MultiSweepAudioProcessor::processBlock(AudioSampleBuffer& buffer,
                                            MidiBuffer&)
{
  // TODO: magic number
  checkInputAndOutput(this, 1, static_cast<int>(*outputChannelsSetting), false);
  ScopedNoDenormals noDenormals;

  const int totalNumInputChannels = getTotalNumInputChannels();
  const int totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  for (int channel = 0; channel < totalNumInputChannels; ++channel) {
    float* channelData = buffer.getWritePointer(channel);
    ignoreUnused(channelData);
    // ..do something to the data...
  }

  sweep.getNextAudioBlock(AudioSourceChannelInfo(buffer));
}

AudioProcessorEditor* MultiSweepAudioProcessor::createEditor()
{
  return new MultiSweepAudioProcessorEditor(*this, parameters);
}

void MultiSweepAudioProcessor::getStateInformation(MemoryBlock& destData)
{
  auto state = parameters.copyState();

  auto oscConfig = state.getOrCreateChildWithName("OSCConfig", nullptr);
  oscConfig.copyPropertiesFrom(oscParameterInterface.getConfig(), nullptr);

  std::unique_ptr<XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void MultiSweepAudioProcessor::setStateInformation(const void* data,
                                                   int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(parameters.state.getType())) {
      parameters.replaceState(ValueTree::fromXml(*xmlState));
      if (parameters.state.hasProperty("OSCPort")) // legacy
      {
        oscParameterInterface.getOSCReceiver().connect(
          parameters.state.getProperty("OSCPort", var(-1)));
        parameters.state.removeProperty("OSCPort", nullptr);
      }

      auto oscConfig = parameters.state.getChildWithName("OSCConfig");
      if (oscConfig.isValid())
        oscParameterInterface.setConfig(oscConfig);
    }
}

void MultiSweepAudioProcessor::parameterChanged(const String& parameterID,
                                                float newValue)
{
  DBG("Parameter with ID " << parameterID
                           << " has changed. New value: " << newValue);

  if (parameterID == "outputChannelsSetting")
    userChangedIOSettings = true;
}

void MultiSweepAudioProcessor::updateBuffers()
{
  DBG("IOHelper:  input size: " << input.getSize());
  DBG("IOHelper: output size: " << output.getSize());
}

void MultiSweepAudioProcessor::playSweep()
{
  sweep.playFromStart();
}

std::vector<std::unique_ptr<RangedAudioParameter>>
MultiSweepAudioProcessor::createParameterLayout()
{
  // add your audio parameters here
  std::vector<std::unique_ptr<RangedAudioParameter>> params;

  params.push_back(OSCParameterInterface::createParameterTheOldWay(
    "outputChannelsSetting",
    "Number of output channels ",
    "",
    NormalisableRange<float>(0.0f, 10.0f, 1.0f),
    0.0f,
    [](float value) { return value < 0.5f ? "Auto" : String(value); },
    nullptr));

  params.push_back(OSCParameterInterface::createParameterTheOldWay(
    "param1",
    "Parameter 1",
    "",
    NormalisableRange<float>(-10.0f, 10.0f, 0.1f),
    0.0,
    [](float value) { return String(value); },
    nullptr));

  params.push_back(OSCParameterInterface::createParameterTheOldWay(
    "param2",
    "Parameter 2",
    "dB",
    NormalisableRange<float>(-50.0f, 0.0f, 0.1f),
    -10.0,
    [](float value) { return String(value, 1); },
    nullptr));

  return params;
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new MultiSweepAudioProcessor();
}
