#include "flto.h"
void bar()
{
  printf("In bar()\n");
  bar();
}
