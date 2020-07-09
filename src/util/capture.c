#include <stdio.h>
#include "utarray.h"
#define DATA_SIZE 1024

void capture(const char* cmdline, UT_array** p_resultA)
{
  FILE* fp;
  fp = popen(cmdline,"r");
  UT_array* resultA = *p_resultA;

  char* eof;
  char data[DATA_SIZE];
  while( (eof = fgets(data, DATA_SIZE, fp)) != NULL)
    utarray_push_back(resultA, data);
  pclose(fp);
}


