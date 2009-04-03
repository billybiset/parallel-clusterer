#include "waiting.h"

void wait ( int miliseconds )
{
  clock_t endwait;
  endwait = clock () + CLOCKS_PER_SEC * (miliseconds / 1000);
  while (clock() < endwait) {}
}
