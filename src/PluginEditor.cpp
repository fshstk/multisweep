#include "PluginEditor.h"
#include "PluginProcessor.h"

MultiSweepAudioProcessorEditor::MultiSweepAudioProcessorEditor(
  MultiSweepAudioProcessor& p,
  juce::AudioProcessorValueTreeState& vts)
  : AudioProcessorEditor(&p)
  , audioProcessor(p)
  , valueTreeState(vts)
  , sweepEditor(p.sweep, vts)
{
  setSize(editorWindowWidth, editorWindowHeight);
  addAndMakeVisible(sweepEditor);
}

void MultiSweepAudioProcessorEditor::paint(juce::Graphics&)
{
}

void MultiSweepAudioProcessorEditor::resized()
{
  auto area = getLocalBounds();
  sweepEditor.setBounds(area);
}
