#include <stdio.h>
#include <stdlib.h>
#define SZ 1000
int main()
{
  int *a = (int *) malloc(SZ*sizeof(int));
  printf("Hello World! a:%d\n",a[0]);
  return 0;
}

