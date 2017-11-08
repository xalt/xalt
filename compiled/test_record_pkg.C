#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "capture.h"

int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      fprintf(stderr,"Need the full path to xalt_record_pkg => quiting!\n");
      exit(1);
    }

  char* run_uuid = getenv("XALT_RUN_UUID");
  if (! run_uuid)
    exit(0);

  char* record_prgm = argv[1];

  std::string cmd;
  std::string syshost;

  cmd.assign();
  cmd.append(" ");
  
    
    

  cap

  
  return 0;
}
