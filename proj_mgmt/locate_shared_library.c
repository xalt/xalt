#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <link.h>
#include <limits.h>
#include <dlfcn.h>
#include "xalt_header.h"
#ifdef HAVE_UUID_UUID_H
#  include <uuid/uuid.h>
#endif
#ifdef HAVE_UUID_H
#  include <uuid.h>
#endif
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
  
