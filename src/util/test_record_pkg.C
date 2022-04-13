#include <stdio.h>
#include <stdlib.h>
#include <string>

int main(int argc, char* argv[])
{
  char* run_uuid = getenv("XALT_RUN_UUID");
  if (! run_uuid)
    {
      fprintf(stderr,"Leaving test_record_pkg because XALT_RUN_UUID is not set -> no records\n");
      exit(0);
    }

  char* xaltDir = getenv("XALT_DIR");

  std::string cmd;

  unsetenv("LD_PRELOAD");

  cmd.assign("XALT_EXECUTABLE_TRACKING=yes ");
  cmd.append(xaltDir);
  cmd.append("/libexec/xalt_record_pkg -u ");
  cmd.append(run_uuid);
  cmd.append(" program R");
  cmd.append(" xalt_run_uuid ");
  cmd.append(run_uuid);
  cmd.append(" package_name acme");
  cmd.append(" package_version 1.0");
  cmd.append(" package_path /opt/apps/intel18/R/3.5/acme/1.0");
  
  fprintf(stderr,"%s\n",cmd.c_str());
    
  system(cmd.c_str());

  cmd.assign("XALT_EXECUTABLE_TRACKING=yes ");
  cmd.append(xaltDir);
  cmd.append("/libexec/xalt_record_pkg -u ");
  cmd.append(run_uuid);
  cmd.append(" program R");
  cmd.append(" xalt_run_uuid ");
  cmd.append(run_uuid);
  cmd.append(" package_name foo");
  cmd.append(" package_version 2.0");
  cmd.append(" package_path /opt/apps/intel18/R/3.5/foo/2.0");
  
  fprintf(stderr,"%s\n",cmd.c_str());
    
  system(cmd.c_str());

  cmd.assign("XALT_EXECUTABLE_TRACKING=yes ");
  cmd.append(xaltDir);
  cmd.append("/libexec/xalt_record_pkg -u ");
  cmd.append(run_uuid);
  cmd.append(" program R");
  cmd.append(" xalt_run_uuid ");
  cmd.append(run_uuid);
  cmd.append(" package_name bar");
  cmd.append(" package_version 3.0");
  cmd.append(" package_path /opt/apps/intel18/R/3.5/bar/3.0");

  cmd.assign("XALT_EXECUTABLE_TRACKING=yes ");
  cmd.append(xaltDir);
  cmd.append("/libexec/xalt_record_pkg -u ");
  cmd.append(run_uuid);
  cmd.append(" program R");
  cmd.append(" xalt_run_uuid ");
  cmd.append(run_uuid);
  cmd.append(" package_name methods");
  cmd.append(" package_version 3.0");
  cmd.append(" package_path /opt/apps/intel18/R/3.5/bar/3.0");
  
  fprintf(stderr,"%s\n",cmd.c_str());
    
  system(cmd.c_str());

  return 0;
}
