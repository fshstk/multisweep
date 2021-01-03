#include "../Source/Sweep.h"
#include <juce_core/juce_core.h>

int main(int argc, char* argv[])
{
  const auto sweepLength = 4;
  const auto fs = 44100;

  const auto sweep = Sweep(fs).linear(sweepLength);
  DBG(sweep.size());

  juce::ignoreUnused(argc, argv);
  return 0;
}
