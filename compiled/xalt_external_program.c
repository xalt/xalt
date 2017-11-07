#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "transmit.h"

int main(int argc, char* argv[])
{
  // Count size of string:
  int i;
  int sz = 2;  // {}
  for (i = 1; i < argc; ++i)
    sz += strlen(argv[i]) + 3;

  char* result = (char *)malloc(sz);

  int len;
  int idx = 0;
  result[idx++] = '{';
  for (i = 1; i < argc; i+=2)
    {
      len           = strlen(argv[i]);
      memcpy(&result[idx],argv[i],len);
      idx          += len;
      result[idx++] = ':';
      len           = strlen(argv[i]);
      memcpy(&result[idx],argv[i+1],len);
      idx          += len;
      result[idx++] = ',';
    }
  result[--idx] = '}';
      
  printf("%s\n",result);


  return 0;
}

//******************
//{"a":"b","c":"d"}
