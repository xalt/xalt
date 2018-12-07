#define _GNU_SOURCE
#include <stdio.h>
#include "build_xalt_tmpdir.h"

char* build_xalt_tmpdir(const char* run_uuid)
{
  char* xalt_tmpdir = NULL;
  asprintf(&xalt_tmpdir,"%s/XALT_pkg_%s",XALT_TMPDIR,run_uuid);
  return xalt_tmpdir;
}
