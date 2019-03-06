#include <stdlib.h>
#include <string.h>
#include "run_submission.h"
#include "xalt_config.h"
#include "epoch.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "xalt_fgets_alloc.h"


void parseProcMaps(pid_t pid, std::vector<Libpair>& libA, double& t_ldd, double& t_sha1)
{
  std::string path;
  char *      buf  = NULL;
  size_t      sz   = 0;
  char *      fn;

  asprintf(&fn,"/proc/%d/maps",pid);

  FILE* fp = fopen(fn,"r");
  if (!fp) return;


  while(xalt_fgets_alloc(fp, &buf, &sz))
    {
      // find the start of the file name
      char *p = strstr(buf,"    /");
      
        

    }

  free(buf); sz = 0; buf = NULL;
