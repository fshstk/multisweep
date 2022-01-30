#include "../src/LogSweep.h"
#include "../src/fft.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

// Generate unique filename so we don't append to existing files:
const auto timestamp = juce::Time::getCurrentTime().toISO8601(false);

const auto basePath = "~/Documents/Toningenieur/Algorithmen 2/sweeptest/";
const auto fs = 44100;
const auto sweepLength = 2;

void saveFile(const juce::AudioSampleBuffer&, std::string);
void saveFile(const std::vector<float>&, std::string);
juce::AudioSampleBuffer makeBufferFromVector(const std::vector<float>&);

// =============================================================================

int main()
{
  DBG(timestamp);

  const auto sweepObject =
    LogSweep(Frequency{ fs }, Duration{ sweepLength }, FreqRange{ 20, fs / 2 });

  const auto sweep = sweepObject.generateSignal();
  const auto invSweep = sweepObject.generateInverse();

  saveFile(sweep, "sweep.wav");
  saveFile(invSweep, "inv_sweep.wav");

  return 0;
}

// =============================================================================

juce::AudioSampleBuffer makeBufferFromVector(const std::vector<float>& vector)
{
  juce::AudioSampleBuffer buffer(1, int(vector.size()));
  for (size_t i = 0; i < vector.size(); ++i)
    buffer.setSample(0, int(i), vector[i]);
  return buffer;
}

void saveFile(const juce::AudioSampleBuffer& buffer, std::string fileName)
{
  const auto filePath = basePath + timestamp + "_" + fileName;

  // The writer object takes ownership of the FileOutputStream, so we don't
  // need to delete it:
  std::unique_ptr<juce::AudioFormatWriter> writer(
    juce::WavAudioFormat().createWriterFor(
      new juce::FileOutputStream(juce::File(filePath)), fs, 1, 24, {}, 0));

  writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
}

void saveFile(const std::vector<float>& vector, std::string fileName)
{
  saveFile(makeBufferFromVector(vector), fileName);
}
