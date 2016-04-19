#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char* argv[])
{
  if (argc > 2)
    {
      char * path =  canonicalize_file_name(argv[1]);
      fprintf(stdout,"%s\n",path);
      free(path);
    }
  
  return 0;
}
