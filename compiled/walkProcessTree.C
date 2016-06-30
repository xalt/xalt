#include "walkProcessTree.h"
#include "xalt_types.h"
#include "Process.h"
#include <stdio.h>
#include <string.h>

void walkProcessTree(pid_t ppid)
{
  pid_t   my_pid       = ppid;
  char *  p_dbg        = getenv("XALT_TRACING");
  int     xalt_tracing = (p_dbg && strcmp(p_dbg,"yes") == 0);
  Vstring cmdlineA;
  std::string name;
  std::string path;
    
  std::vector<ProcessTree> ptV;

  fprintf(stderr, "\nStart of walkProcessTree\n");

  while(1)
    {
      Process proc(my_pid);
      pid_t   parent       = proc.parent();
      if (parent == 1) break;

      name.assign(proc.name());
      path.assign(proc.exe());
      proc.cmdline(cmdlineA);

      ProcessTree pt(name, path, cmdlineA);
      ptV.push_back(pt);
      my_pid = parent;
    }

  
  for (auto it = ptV.begin(); it != ptV.end(); ++it)
    {
      const std::string& name = (*it).name;
      const std::string& path = (*it).path;

      //if (xalt_tracing)
        fprintf(stderr, "  my_name: %s, path: %s\n", name.c_str(), path.c_str());
    }

  fprintf(stderr, "\nEnd of walkProcessTree\n");

}

