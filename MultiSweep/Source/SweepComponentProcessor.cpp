#include "SweepComponentProcessor.h"
#include "SweepComponentEditor.h"

// TODO: we might not even need this?
juce::AudioProcessorEditor* SweepComponentProcessor::createEditor()
{
  return new SweepComponentEditor(*this);
}
