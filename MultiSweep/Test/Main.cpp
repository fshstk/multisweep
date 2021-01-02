#include "../Source/Sweep.h"
#include <juce_core/juce_core.h>

int main(int argc, char* argv[])
{
  const auto sweepLength = 4;

  // TODO: doing it this way gets me an address boundary error. why?
  // const auto sweep = Sweep(44100).linear(sweepLength);

  const auto sweepGenerator = Sweep(44100);
  const auto sweep = sweepGenerator.linear(sweepLength);

  DBG(sweep.size());

  juce::ignoreUnused(argc, argv);
  return 0;
}
