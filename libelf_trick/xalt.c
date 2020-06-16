#include <stdio.h>
void myinit(int argc, char **argv)
{
  printf("This is run before main()\n");
}
void myfini()
{
  printf("This is run after main()\n");
}

__attribute__((section(".init_array"))) __typeof__(myinit) *__init = myinit;
__attribute__((section(".fini_array"))) __typeof__(myfini) *__fini = myfini;
