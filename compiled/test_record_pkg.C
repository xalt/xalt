#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "capture.h"

int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      fprintf(stderr,"Need the full path to the directory for xalt_record_pkg => quiting!\n");
      exit(1);
    }

  char* run_uuid = getenv("XALT_RUN_UUID");
  if (! run_uuid)
    exit(0);

  char* xaltDir = argv[1];

  std::string cmd;
  std::string syshost;

  cmd.assign(xaltDir);
  cmd.append("/libexec/xalt_syshost");

  Vstring v;
    
    

  capture(cmd,v);
  std::string syshost = v[0];

  cmd.assign(xaltDir);
  cmd.append("/libexec/xalt_record_pkg -s ");
  cmd.append(syshost);
  cmd.append(" -u ");
  cmd.append(run_uuid);
  cmd.append(" program R");
  cmd.append(" xalt_run_uuid ");
  cmd.append(run_uuid);
  cmd.append(" package_name acme");
  cmd.append(" package_verion 1.0");
  cmd.append(" package_file /A/B/acme/1.0");
  
  printf("%s\n",cmd.c_str());
    
  return 0;
}
