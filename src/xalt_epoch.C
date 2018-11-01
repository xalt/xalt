#include "epoch.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[])
{
  double t1 = epoch();
  if (argc == 1)
    {
      printf("%.4f\n", t1);
      return 0;
    }

  double t0 = strtod(argv[1],(char **) NULL);

  printf("%.4f\n", t1 - t0);
  return 0;
}
