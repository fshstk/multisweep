#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "StereoPluginBase.h"

namespace {
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  return { params.begin(), params.end() };
}
}

MultiSweepAudioProcessor::MultiSweepAudioProcessor()
  : parameters(*this, nullptr, "Parameters", createParameterLayout())
{}

void MultiSweepAudioProcessor::prepareToPlay(double sampleRate,
                                             int samplesPerBlock)
{
  sweep.prepareToPlay(sampleRate, samplesPerBlock);
}

void MultiSweepAudioProcessor::releaseResources()
{
  sweep.releaseResources();
}

void MultiSweepAudioProcessor::processBlock(juce::AudioSampleBuffer& buffer,
                                            juce::MidiBuffer& midi)
{
  juce::ScopedNoDenormals noDenormals;

  const int totalNumInputChannels = getTotalNumInputChannels();
  const int totalNumOutputChannels = getTotalNumOutputChannels();

  for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  sweep.processBlock(buffer, midi);
}

juce::AudioProcessorEditor* MultiSweepAudioProcessor::createEditor()
{
  return new MultiSweepAudioProcessorEditor(*this, parameters);
}
