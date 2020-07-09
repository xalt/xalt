#include <stdio.h>
#include "utarray.h"
#define DATA_SIZE 1024

void capture(const char* cmdline, UT_array** p_resultA)
{
  UT_array* resultA;
  FILE* fp;
  char* eof;
  char data[DATA_SIZE];
  char *s = &data[0]; 

  utarray_new(resultA, &ut_str_icd);
  fp = popen(cmdline,"r");
  while( (eof = fgets(data, DATA_SIZE, fp)) != NULL)
    utarray_push_back(resultA, &s);
  pclose(fp);
  *p_resultA = resultA;
}
