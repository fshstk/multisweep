#include "PluginEditor.h"
#include "PluginProcessor.h"

MultiSweepAudioProcessorEditor::MultiSweepAudioProcessorEditor(
  MultiSweepAudioProcessor& p,
  AudioProcessorValueTreeState& vts)
  : AudioProcessorEditor(&p)
  , audioProcessor(p)
  , valueTreeState(vts)
  , footer()
  , sweepEditor(p.sweep, vts)
{
  setSize(editorWindowWidth, editorWindowHeight);
  setLookAndFeel(&iemLookAndFeel);

  addAndMakeVisible(title);
  addAndMakeVisible(footer);

  title.setTitle(String("Multi"), String("Sweep"));
  title.setFont(iemLookAndFeel.robotoBold, iemLookAndFeel.robotoLight);

  outputChannelsAttachment = std::make_unique<ComboBoxAttachment>(
    valueTreeState,
    "outputChannelsSetting",
    *title.getOutputWidgetPtr()->getChannelsCbPointer());

  addAndMakeVisible(sweepEditor);

  startTimer(20); // --> timerCallback()
}

MultiSweepAudioProcessorEditor::~MultiSweepAudioProcessorEditor()
{
  setLookAndFeel(nullptr);
}

void MultiSweepAudioProcessorEditor::paint(Graphics& g)
{
  g.fillAll(iemLookAndFeel.ClBackground);
}

void MultiSweepAudioProcessorEditor::resized()
{
  Rectangle<int> area = getLocalBounds();
  drawHeaderFooter(area);
  sweepEditor.setBounds(area);
}

void MultiSweepAudioProcessorEditor::timerCallback()
{
  updateIOChannelCount();
}

void MultiSweepAudioProcessorEditor::updateIOChannelCount()
{
  title.setMaxSize(audioProcessor.getMaxSize());
}

void MultiSweepAudioProcessorEditor::drawHeaderFooter(Rectangle<int>& canvas)
{
  const int leftRightMargin = 30;
  const int headerHeight = 60;
  const int footerHeight = 25;

  Rectangle<int> footerArea(canvas.removeFromBottom(footerHeight));
  footer.setBounds(footerArea);

  canvas.removeFromLeft(leftRightMargin);
  canvas.removeFromRight(leftRightMargin);
  Rectangle<int> headerArea = canvas.removeFromTop(headerHeight);
  title.setBounds(headerArea);
  canvas.removeFromTop(10);
  canvas.removeFromBottom(5);
}
