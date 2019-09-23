#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <link.h>
#include <limits.h>
#include <dlfcn.h>
#include "uuid/uuid.h"
int main(int argc, char** argv)
{
  void *handle;
  struct link_map * map;

  if (argc < 1)
    {
      fprintf(stderr, "No argument given -> exiting\n");
      exit(-1);
    }

  
  handle = dlopen (argv[1], RTLD_LAZY);
  if (! handle)
    exit(1);

  int result = dlinfo(handle, RTLD_DI_LINKMAP,  &map);
  char * name = realpath(map->l_name, NULL);
  printf("%s:%s\n",map->l_name, name);
}
  
