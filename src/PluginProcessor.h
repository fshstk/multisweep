#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class MultiSweepAudioProcessor : public juce::AudioProcessor
{
public:
  MultiSweepAudioProcessor();
  ~MultiSweepAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  juce::AudioProcessorEditor* createEditor() override;

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  // Boilerplate functions:
  bool hasEditor() const override { return true; };
  const juce::String getName() const override { return JucePlugin_Name; };
  bool acceptsMidi() const override { return false; };
  bool producesMidi() const override { return false; };
  bool isMidiEffect() const override { return false; };
  double getTailLengthSeconds() const override { return 0.0; };
  int getNumPrograms() override { return 1; };
  int getCurrentProgram() override { return 0; };
  void setCurrentProgram(int index) override{};
  const juce::String getProgramName(int index) override { return {}; };
  void changeProgramName(int index, const juce::String& newName) override{};

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSweepAudioProcessor)
};
