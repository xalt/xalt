#include "xalt_quotestring.h"
#include <stdio.h>
#include <string.h>

const int sz = 1024;
char buf[sz];

const char* ans1 = "./hello\\u00f1";
const char* ans2 = "./helloñ";

int main(int argc, char* argv[])
{
  const char* s = "./helloñ";
  const char* p = xalt_quotestring(s);
  strcpy(buf, p);

  if (strcmp(buf, ans1) != 0)
    {
      printf("failed part 1\n");
      return 1;
    }

  int len = strlen(buf);
  
  p = xalt_unquotestring(buf, len);

  if (strcmp(p, ans2) != 0)
    {
      printf("failed part 2\n");
      return 1;
    }

  return 0;
}

