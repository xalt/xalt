#include "epoch.h"

double epoch()
{
  struct timeval tm;
  gettimeofday(&tm, NULL);
  return tm.tv_sec + 1.0e-6*tm.tv_usec;
}
