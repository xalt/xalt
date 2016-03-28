#include "link_submission.h"
#include "Process.h"

void extract_linker(std::string& compiler, std::string& compilerPath, std::vector<std::string>& linklineA)
{

  //--------------------------------------------------
  // Walk process tree to find compiler name and path

  std::string ignorePrgA  = {"ld", "collect2","bash","Python", "python", "sh" };
  int         ignorePrgSz = sizeof(ignorePrgA)/sizeof(ignorePrgA[0]);

  std::string compiler     = "unknown";
  std::string compilerPath = "unknown";

  pid_t my_pid = getpid();
  while(1)
    {
      Process proc(my_pid);
      pid_t parent = proc.parent()
      if (parent == 1) break;

      std::string name = proc.name();
      bool found = false;
      for (int i = 0; i < ignorePrgSz; ++i)
        {
          if (name == ignorePrgA[i])
            {
              found = true;
              break;
            }
        }
      
      if (found)
        continue;

      // If here then we have found our guess at the name of the compiler
      compiler     = name;
      compilerPath = proc.exe();
      proc.cmdline(linklineA);
      break;
    }
}
