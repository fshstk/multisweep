/*
 ==============================================================================
 This file is part of the IEM plug-in suite.
 Authors: Fabian Hummel, David Neussl
 Copyright (c) 2020 - Institute of Electronic Music and Acoustics (IEM)
 https://iem.at

 The IEM plug-in suite is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 The IEM plug-in suite is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this software.  If not, see <https://www.gnu.org/licenses/>.
 ==============================================================================
 */

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

class Sweep
{
public:
  struct FreqRange : juce::Range<double>
  {
    FreqRange()
      : juce::Range<double>(min, max)
    {}

    static constexpr double min = 20;
    static constexpr double max = 20e3;
  };

public:
  Sweep(juce::AudioProcessor* _audioContext);
  Sweep(double sampleRate);
  ~Sweep() = default;

  juce::AudioSampleBuffer linear(float duration,
                                 FreqRange range = FreqRange()) const;
  juce::AudioSampleBuffer inverseLinear(float duration,
                                        FreqRange range = FreqRange()) const;
  juce::AudioSampleBuffer exponential(float duration,
                                      FreqRange range = FreqRange()) const;
  juce::AudioSampleBuffer inverseExponential(
    float duration,
    FreqRange range = FreqRange()) const;

private:
  enum SweepType // needs to be declared before generateSweep()
  {
    Linear,
    Exponential
  };

private:
  double getSampleRate() const;
  juce::AudioSampleBuffer generateSweep(float duration,
                                        FreqRange range,
                                        SweepType type,
                                        bool inverse) const;

private:
  const juce::AudioProcessor* const audioContext;
  const double sampleRate;
  static constexpr auto pi = juce::MathConstants<double>::pi;
};
