#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "xalt_config.h"

const char* xalt_dir(const char* file)
{
  int i;
  char *joined;
  const char* dirA[2];

  dirA[0] = getenv("XALT_DIR");
  dirA[1] = XALT_DEFAULT_DIR;

  for (i = 0; i < 2; ++i)
    {
      const char* dir = dirA[i];
      if (dir == NULL)
        continue;

      if (!file) 
        return dir;

      int ret = asprintf(&joined, "%s/%s", dir, file);
      if (ret > 0 && access(joined, R_OK) != -1)
        {
          /* this will leak memory if the caller does not free() the string */
          return joined;
        }
    }
  return file;
}
