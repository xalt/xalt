#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include "xalt_config.h"

const char* xalt_dir(const char* file) {
  const char* dir = getenv("XALT_DIR") ? getenv("XALT_DIR") : XALT_DEFAULT_DIR;

  if (!file) {
    return dir;
  }
  else {
    char *joined;
    int ret = asprintf(&joined, "%s/%s", dir, file);
    if (ret > 0) {
      if (access(joined, F_OK) != 0) 
        syslog(LOG_WARNING, "XALT unable to access '%s'", joined);

      /* this will leak memory if the caller does not free() the string */
      return joined;
    }
    else {
      return file;
    }
  }
}
