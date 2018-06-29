#include <stdio.h>
#include <stdlib.h>
#include "capture.h"

int main(int argc, char* argv[])
{
  char* run_uuid = getenv("XALT_RUN_UUID");
  if (! run_uuid)
    exit(0);

  char* xaltDir = getenv("XALT_DIR");

  std::string cmd;

  cmd.assign(xaltDir);
  cmd.append("/libexec/xalt_record_pkg -u ");
  cmd.append(run_uuid);
  cmd.append(" program R");
  cmd.append(" xalt_run_uuid ");
  cmd.append(run_uuid);
  cmd.append(" package_name acme");
  cmd.append(" package_version 1.0");
  cmd.append(" package_path /A/B/acme/1.0");
  
  printf("%s\n",cmd.c_str());
    
  system(cmd.c_str());

  cmd.assign(xaltDir);
  cmd.append("/libexec/xalt_record_pkg -u ");
  cmd.append(run_uuid);
  cmd.append(" program R");
  cmd.append(" xalt_run_uuid ");
  cmd.append(run_uuid);
  cmd.append(" package_name foo");
  cmd.append(" package_version 2.0");
  cmd.append(" package_path /A/B/foo/2.0");
  
  printf("%s\n",cmd.c_str());
    
  system(cmd.c_str());

  cmd.assign(xaltDir);
  cmd.append("/libexec/xalt_record_pkg -u ");
  cmd.append(run_uuid);
  cmd.append(" program R");
  cmd.append(" xalt_run_uuid ");
  cmd.append(run_uuid);
  cmd.append(" package_name bar");
  cmd.append(" package_version 3.0");
  cmd.append(" package_path /A/B/bar/3.0");
  
  printf("%s\n",cmd.c_str());
    
  system(cmd.c_str());

  return 0;
}
