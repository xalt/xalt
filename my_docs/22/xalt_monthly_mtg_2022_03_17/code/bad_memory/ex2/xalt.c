#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SZ 1000
void myinit(int argc, char **argv)
{
  int i;
  int *a = (int*) malloc(SZ*sizeof(int));
  for (i = 0; i < SZ; ++i) a[i] = 15; 
  free(a);
  printf("This is run before main()\n");
}
void myfini()
{
  printf("This is run after main()\n");
}

__attribute__((section(".init_array"))) __typeof__(myinit) *__init = myinit;
__attribute__((section(".fini_array"))) __typeof__(myfini) *__fini = myfini;
