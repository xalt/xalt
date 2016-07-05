#include "walkProcessTree.h"
#include "xalt_types.h"
#include "Process.h"
#include <stdio.h>
#include <string.h>

void walkProcessTree(pid_t ppid, std::vector<ProcessTree>& ptV)
{
  pid_t       my_pid = ppid;
  std::string name;
  std::string path;
  while(1)
    {
      Process proc(my_pid);
      pid_t   parent       = proc.parent();
      if (parent == 1) break;

      name.assign(proc.name());
      path.assign(proc.exe());

      Vstring cmdlineA;
      proc.cmdline(cmdlineA);

      ProcessTree pt(name, path, cmdlineA);
      ptV.push_back(pt);
      my_pid = parent;
    }
}
