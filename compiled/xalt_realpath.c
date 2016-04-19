#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
int main(int argc, char* argv[])
{
  if (argc > 1)
    {
      char * path = argv[1];
      if (access(path, F_OK) == 0)
        {
          path =  canonicalize_file_name(path);
          fprintf(stdout,"%s\n",path);
          free(path);
        }
      else
        fprintf(stdout,"%s\n",path);
        
    }
  
  return 0;
}
