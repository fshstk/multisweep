#pragma once
#include "SweepComponentProcessor.h"
#include <IEM_LaF.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

class FreqResponseDisplay
  : public juce::Component
  , private juce::ChangeListener
{
public:
  FreqResponseDisplay(SweepComponentProcessor& processor);
  ~FreqResponseDisplay() override;

  FreqResponseDisplay(const FreqResponseDisplay&) = delete;
  FreqResponseDisplay& operator=(const FreqResponseDisplay&) = delete;

  void paint(juce::Graphics& g) override;

private:
  void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
  iem::LaF lookAndFeel;
  SweepComponentProcessor& sweep;
  std::vector<float> sweepResponse;
};
