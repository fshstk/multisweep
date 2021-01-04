#include "../Source/Sweep.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

// Generate unique filename so we don't append to existing files:
const auto timestamp = juce::Time::getCurrentTime().toISO8601(false);

const auto basePath = "~/Documents/Toningenieur/Algorithmen 2/sweeptest/";
const auto fs = 44100;
const auto sweepLength = 2;

void saveFile(juce::AudioSampleBuffer, std::string);

int main(int argc, char* argv[])
{
  DBG(timestamp);

  saveFile(Sweep(fs).linear(sweepLength), "lin.wav");
  saveFile(Sweep(fs).inverseLinear(sweepLength), "inv_lin.wav");
  saveFile(Sweep(fs).exponential(sweepLength), "exp.wav");
  saveFile(Sweep(fs).inverseExponential(sweepLength), "inv_exp.wav");

  juce::ignoreUnused(argc, argv);
  return 0;
}

void saveFile(juce::AudioSampleBuffer buffer, std::string fileName)
{
  const auto filePath = basePath + timestamp + "_" + fileName;

  std::unique_ptr<juce::AudioFormatWriter> writer(
    juce::WavAudioFormat().createWriterFor(
      new juce::FileOutputStream(juce::File(filePath)), fs, 1, 24, {}, 0));

  writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
}
