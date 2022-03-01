#pragma once
#include "LogSweep.h"
#include <juce_audio_utils/juce_audio_utils.h>

struct SweepComponentMetadata
{
  int channel = 0;
  double duration = 2;
  double lowerFreq = 10.0;
  double upperFreq = 22e3;
  double responseTailInSeconds = 1;
};

class SweepComponentProcessor
  : public juce::AudioProcessor
  , public juce::ChangeBroadcaster
{
public:
  SweepComponentProcessor() = default;

  const juce::String getName() const override { return {}; }
  double getTailLengthSeconds() const override { return 0; }

  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }

  int getNumPrograms() override { return 0; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  void changeProgramName(int, const juce::String&) override {}
  const juce::String getProgramName(int) override { return {}; }

  void getStateInformation(juce::MemoryBlock&) override {}
  void setStateInformation(const void*, int) override {}

  bool hasEditor() const override { return true; }
  juce::AudioProcessorEditor* createEditor() override { return nullptr; }

  void prepareToPlay(double sampleRate, int maxSamplesPerBlock) override;
  void processBlock(juce::AudioSampleBuffer& buffer,
                    juce::MidiBuffer&) override;
  void releaseResources() override;

  void startSweep(SweepComponentMetadata metadata);
  void stopSweep();
  void exportFilter() const;
  void clearData();

  std::vector<float> getImpulseResponse();
  std::vector<float> getFrequencyResponse(size_t numbins);

private:
  void saveInputBuffer(juce::AudioSampleBuffer& input);

  template<typename T>
  static juce::AudioBuffer<T> makeBufferFromVector(const std::vector<T>& vector)
  {
    juce::AudioBuffer<T> buffer(1, int(vector.size()));
    for (size_t i = 0; i < vector.size(); ++i)
      buffer.setSample(0, int(i), vector[i]);
    return buffer;
  }

  template<typename T>
  static std::vector<T> makeVectorFromBuffer(const juce::AudioBuffer<T>& buffer)
  {
    const float* const ptr = buffer.getReadPointer(0);
    const auto length = buffer.getNumSamples();
    return std::vector<T>(ptr, ptr + length);
  }

private:
  bool sweepActive = false;   // used in check for recording AND playback
  bool sweepFinished = false; // only used in check for playback
  double fs = 0;
  int samplesPerBlock = 0;

  int outputBufferIndex = 0;
  int inputBufferIndex = 0;

  std::unique_ptr<juce::MemoryAudioSource> audioSource;
  std::unique_ptr<juce::ChannelRemappingAudioSource> outputChannelMapper;

  std::unique_ptr<juce::AudioSampleBuffer> inputBuffer;
  std::unique_ptr<LogSweep> sweep;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SweepComponentProcessor)
};
