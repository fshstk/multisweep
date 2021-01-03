#include "../Source/Sweep.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

int main(int argc, char* argv[])
{
  const auto sweepLength = 4;
  const auto fs = 44100;

  const auto sweep = Sweep(fs).linear(sweepLength);

  juce::WavAudioFormat format;
  std::unique_ptr<juce::AudioFormatWriter> writer;
  writer.reset(format.createWriterFor(
    new juce::FileOutputStream(juce::File("~/Downloads/sweeptest/test.wav")),
    fs,
    sweep.getNumChannels(),
    24,
    {},
    0));
  if (writer != nullptr)
    writer->writeFromAudioSampleBuffer(sweep, 0, sweep.getNumSamples());

  juce::ignoreUnused(argc, argv);
  return 0;
}
