#include <stdio.h>
#include <unistd.h>
static int          errfd          = -1;
void myinit(int argc, char **argv)
{
  errfd = dup(STDERR_FILENO);
  fprintf(stderr,"This is run before main()\n");
}
void myfini()
{
  FILE * my_stderr = NULL;
  fflush(stderr);
  close(STDERR_FILENO);
  dup2(errfd, STDERR_FILENO);
  my_stderr = fdopen(errfd,"w");
  fprintf(my_stderr,"This is run after main()\n");
  fclose(my_stderr);
  close(errfd);
  close(STDERR_FILENO);
}

__attribute__((section(".init_array"))) __typeof__(myinit) *__init = myinit;
__attribute__((section(".fini_array"))) __typeof__(myfini) *__fini = myfini;
