#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "xalt_tmpdir.h"
#include "xalt_config.h"

char* create_xalt_tmpdir_str(const char* run_uuid)
{
  char* xalt_tmpdir = NULL;
  asprintf(&xalt_tmpdir,"%s/XALT_pkg_%s/", XALT_TMPDIR, run_uuid);
  return xalt_tmpdir;
}


void remove_xalt_tmpdir(const char* run_uuid)
{
  char* xalt_tmpdir = create_xalt_tmpdir_str(run_uuid);
  DIR*  dirp        = opendir(xalt_tmpdir);
  if (dirp == NULL)
    {
      free(xalt_tmpdir);
      return;
    }

  struct dirent* dp;
  while ( (dp = readdir(dirp)) != NULL)
    {
      char *fullFn = NULL;
      asprintf(&fullFn,"%s/%s",xalt_tmpdir, dp->d_name);
        
      unlink(fullFn);
      free(fullFn);
    }
  rmdir(xalt_tmpdir);
  free(xalt_tmpdir);
}

