#include "FreqResponseDisplay.h"

FreqResponseDisplay::FreqResponseDisplay(SweepComponentProcessor& processor)
  : sweep(processor)
{
  sweep.addChangeListener(this);
}

FreqResponseDisplay::~FreqResponseDisplay()
{
  sweep.removeChangeListener(this);
}

void FreqResponseDisplay::paint(juce::Graphics& g)
{

  const auto margin = 35;
  const auto area = getLocalBounds();

  const auto graph = area.reduced(margin);
  const auto xAxis = dft_log_bins(size_t(graph.getWidth()), 20.0, 20e3);

  const auto numPixels = graph.getWidth();

  // TODO: The way this is coded right now means that the frequency response
  // is re-calculated EVERY TIME the UI is repainted. This is barely
  // noticeable performance wise but is a disaster and needs to be fixed ASAP.
  const auto& curve =
    sweep.getFrequencyResponse(static_cast<size_t>(numPixels));

  auto yAxis = std::vector<float>(size_t(graph.getHeight()));
  std::iota(yAxis.begin(), yAxis.end(), 0);
  std::transform(yAxis.cbegin(), yAxis.cend(), yAxis.begin(), [&yAxis](auto x) {
    const auto dB_low = -24;
    const auto dB_high = 24;
    return x * (dB_high - dB_low) / yAxis.size() + dB_low;
  });
  const auto yValues =
    std::vector<float>{ -24, -18, -12, -6, 0, 6, 12, 18, 24 };
  const auto findPixelForDb = [&yAxis](auto value) {
    return std::distance(yAxis.cbegin(),
                         std::lower_bound(yAxis.cbegin(), yAxis.cend(), value));
  };
  auto yPixels = std::vector<int>(yValues.size());
  std::transform(
    yValues.cbegin(), yValues.cend(), yPixels.begin(), findPixelForDb);

  const auto values =
    std::vector<float>{ 20,  30,  40,  50,  60,  70,  80,   90,  100, 200,
                        300, 400, 500, 600, 700, 800, 900,  1e3, 2e3, 3e3,
                        4e3, 5e3, 6e3, 7e3, 8e3, 9e3, 10e3, 20e3 };
  const auto labeledValues = std::vector<float>{ 20, 200, 2e3, 20e3 };

  const auto findPixelForFrequency = [&xAxis](auto value) {
    return std::distance(xAxis.cbegin(),
                         std::lower_bound(xAxis.cbegin(), xAxis.cend(), value));
  };

  auto xPixels = std::vector<int>(values.size());
  std::transform(
    values.cbegin(), values.cend(), xPixels.begin(), findPixelForFrequency);

  g.setColour(juce::Colours::darkslategrey);
  g.fillRect(graph);
  g.setColour(lookAndFeel.ClSeperator);

  for (auto& pixel : xPixels) {
    const auto x = pixel + graph.getX();
    const auto y1 = graph.getY();
    const auto y2 = graph.getBottom();

    g.drawLine(x, y1, x, y2);
  }

  for (auto& pixel : yPixels) {
    const auto y = pixel + graph.getY();
    const auto x1 = graph.getX();
    const auto x2 = graph.getRight();

    g.drawLine(x1, y, x2, y);
  }

  for (auto& value : labeledValues) {
    const auto x = graph.getX() + findPixelForFrequency(value);
    const auto y = graph.getBottom() + margin / 2;

    const auto label = value < 1000 ? juce::String(value) + " Hz"
                                    : juce::String(value / 1000) + " kHz";

    g.setFont(lookAndFeel.robotoMedium);
    g.setColour(lookAndFeel.ClText);
    g.drawSingleLineText(
      label, int(x), int(y), juce::Justification::horizontallyCentred);
  }

  for (auto& value : yValues) {
    const auto x = graph.getX() - 5;
    const auto y = graph.getBottom() - findPixelForDb(value) + 3;

    const auto label = juce::String(value) + " dB";

    g.setFont(lookAndFeel.robotoMedium);
    g.setColour(lookAndFeel.ClText);
    g.drawSingleLineText(label, int(x), int(y), juce::Justification::right);
  }

  const auto zeroDbCurve = std::vector<float>(xAxis.size(), 0);
  const auto plusTenDbCurve = std::vector<float>(xAxis.size(), 10);
  const auto minusTenDbCurve = std::vector<float>(xAxis.size(), -10);

  auto path = juce::Path();
  path.startNewSubPath(graph.getX() + findPixelForFrequency(20),
                       graph.getBottom() - findPixelForDb(0));

  const auto drawPoint = [&](auto frequency, auto db) {
    const auto x = graph.getX() + findPixelForFrequency(frequency);
    const auto y = graph.getBottom() - findPixelForDb(db);
    path.lineTo(x, y);
  };

  for (auto i = 0U; i < curve.size(); ++i)
    drawPoint(xAxis[i], curve[i]);

  g.setColour(lookAndFeel.ClSeperator);
  g.drawRect(graph);

  if (curve.size() == 0) {
    g.setColour(juce::Colours::white);
    g.setOpacity(0.4f);
    g.fillRect(graph);
    g.setColour(juce::Colours::darkslategrey);
    g.setFont(lookAndFeel.robotoBold);
    g.setFont(20);
    g.drawText("No Sweep Recorded", graph, juce::Justification::centred);
  }

  g.reduceClipRegion(graph.reduced(1)); // reduce by stroke width
  g.setColour(juce::Colours::red);
  g.strokePath(path, juce::PathStrokeType(2.0f));
}

void FreqResponseDisplay::changeListenerCallback(juce::ChangeBroadcaster*)
{
  repaint();
}
