#include "SweepComponentProcessor.h"
#include "SweepComponentEditor.h"

juce::AudioProcessorEditor* SweepComponentProcessor::createEditor()
{
  return new SweepComponentEditor(this);
}
