#pragma once
#include <customComponents/TitleBar.h>

template<int maxChannels, bool selectable = true>
class SingleChannelIOWidget
  : public IOWidget
  , private ComboBox::Listener
{
public:
  SingleChannelIOWidget()
    : IOWidget()
  {
    WaveformPath.loadPathFromData(WaveformPathData, sizeof(WaveformPathData));
    setBufferedToImage(true);

    if (selectable) {
      cbChannels.reset(new ComboBox());
      addAndMakeVisible(cbChannels.get());
      cbChannels->setJustificationType(Justification::centred);
      cbChannels->addSectionHeading("Select Channel for Sweep");
      for (int i = 1; i <= maxChannels; ++i)
        cbChannels->addItem(String(i), i + 1);
      cbChannels->setBounds(35, 8, 70, 15);
      cbChannels->addListener(this);
    }
  }

  const int getComponentSize() override { return selectable ? 110 : 75; }

  void updateDisplayTextIfNotSelectable()
  {
    if (availableChannels < channelSizeIfNotSelectable) {
      displayTextIfNotSelectable =
        String(channelSizeIfNotSelectable) + " (bus too small)";
      setBusTooSmall(true);
    } else {
      displayTextIfNotSelectable = String(channelSizeIfNotSelectable);
      setBusTooSmall(false);
    }
    repaint();
  }

  void checkIfBusIsTooSmall()
  {
    if (availableChannels < cbChannels->getSelectedId() - 1)
      setBusTooSmall(true);
    else
      setBusTooSmall(false);
  }

  void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
  {
    ignoreUnused(comboBoxThatHasChanged);
    checkIfBusIsTooSmall();
  }

  void setMaxSize(int maxPossibleNumberOfChannels) override
  {
    if (availableChannels != maxPossibleNumberOfChannels) {
      availableChannels = maxPossibleNumberOfChannels;
      if (selectable) {
        if (maxPossibleNumberOfChannels > 0)
          cbChannels->changeItemText(
            1, "Auto (" + String(maxPossibleNumberOfChannels) + ")");
        else
          cbChannels->changeItemText(1, "(Auto)");
        int currId = cbChannels->getSelectedId();
        if (currId == 0)
          currId = 1; // bad work around
        int i;
        for (i = 1; i <= maxPossibleNumberOfChannels; ++i) {
          cbChannels->changeItemText(i + 1, String(i));
        }
        for (i = maxPossibleNumberOfChannels + 1; i <= maxChannels; ++i) {
          cbChannels->changeItemText(i + 1, String(i) + " (bus too small)");
        }
        checkIfBusIsTooSmall();

        cbChannels->setText(
          cbChannels->getItemText(cbChannels->indexOfItemId(currId)));
      } else {
        updateDisplayTextIfNotSelectable();
      }
    }
  }

  void setSizeIfUnselectable(int newSize)
  {
    if (!selectable && channelSizeIfNotSelectable != newSize) {
      channelSizeIfNotSelectable = newSize;
      updateDisplayTextIfNotSelectable();
    }
  }

  ComboBox* getChannelsCbPointer()
  {
    if (selectable)
      return cbChannels.get();
    return nullptr;
  }

  void paint(Graphics& g) override
  {
    WaveformPath.applyTransform(WaveformPath.getTransformToScaleToFit(
      0, 0, 30, 30, true, Justification::centred));
    g.setColour((Colours::white).withMultipliedAlpha(0.5));
    g.fillPath(WaveformPath);

    if (!selectable) {
      g.setColour((Colours::white).withMultipliedAlpha(0.5));
      g.setFont(getLookAndFeel().getTypefaceForFont(Font(12.0f, 1)));
      g.setFont(15.0f);
      g.drawFittedText(displayTextIfNotSelectable,
                       35,
                       0,
                       40,
                       30,
                       Justification::centredLeft,
                       2);
    }
  }

private:
  std::unique_ptr<ComboBox> cbChannels;
  Path WaveformPath;
  int availableChannels{ 64 };
  int channelSizeIfNotSelectable = maxChannels;
  String displayTextIfNotSelectable = String(maxChannels);
};
