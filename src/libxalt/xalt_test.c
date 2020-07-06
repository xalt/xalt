#include <stdio.h>
void myinit(int argc, char **argv);
void myfini();



int main(int argc, char **argv)
{
  fprintf(stderr,"Calling myinit()\n");
  myinit(argc, argv);

  fprintf(stderr,"Calling myfini()\n");
  myfini();
  fprintf(stderr,"done!\n");
  return 0;
}


