#include "PluginEditor.h"
#include "PluginProcessor.h"

MultiSweepAudioProcessorEditor::MultiSweepAudioProcessorEditor(
  MultiSweepAudioProcessor& p,
  AudioProcessorValueTreeState& vts)
  : AudioProcessorEditor(&p)
  , audioProcessor(p)
  , valueTreeState(vts)
  , sweepEditor(p.sweep, vts)
{
  setSize(editorWindowWidth, editorWindowHeight);
  addAndMakeVisible(sweepEditor);
}

void MultiSweepAudioProcessorEditor::paint(Graphics&)
{
}

void MultiSweepAudioProcessorEditor::resized()
{
  auto area = getLocalBounds();
  sweepEditor.setBounds(area);
}
