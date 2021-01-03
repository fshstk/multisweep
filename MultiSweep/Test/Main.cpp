#include "../Source/Sweep.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

const auto basePath = "~/Downloads/sweeptest/";
const auto fs = 44100;

void saveFile(juce::AudioSampleBuffer, std::string);

int main(int argc, char* argv[])
{
  const auto sweepLength = 1;

  saveFile(Sweep(fs).linear(sweepLength), "lin.wav");
  saveFile(Sweep(fs).inverseLinear(sweepLength), "inv_lin.wav");
  saveFile(Sweep(fs).exponential(sweepLength), "exp.wav");
  saveFile(Sweep(fs).inverseExponential(sweepLength), "inv_exp.wav");

  juce::ignoreUnused(argc, argv);
  return 0;
}

void saveFile(juce::AudioSampleBuffer buffer, std::string fileName)
{
  auto filePath = basePath + fileName;

  std::unique_ptr<juce::AudioFormatWriter> writer(
    juce::WavAudioFormat().createWriterFor(
      new juce::FileOutputStream(juce::File(filePath)), fs, 1, 24, {}, 0));

  writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
}
