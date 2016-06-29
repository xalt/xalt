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
  Vstring linklineA;

  while(1)
    {
      Process proc(my_pid);
      pid_t   parent       = proc.parent();
      if (parent == 1) break;

      std::string name = proc.name();
      std::string path = proc.exe();
      proc.cmdline(linklineA);
      
      if (xalt_tracing)
        fprintf(stderr, "  name: %s, path: %s\n", name.c_str(), path.c_str());
      my_pid = parent;
    }
}

