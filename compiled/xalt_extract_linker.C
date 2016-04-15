#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "xalt_types.h"
#include "Process.h"
#include "Json.h"

void extract_linker(std::string& compiler, std::string& compilerPath, Vstring& linklineA)
{

  //--------------------------------------------------
  // Walk process tree to find compiler name and path

  std::string ignorePrgA[] = {"ld", "collect2","bash","Python", "python", "sh" };
  int         ignorePrgSz  = sizeof(ignorePrgA)/sizeof(ignorePrgA[0]);

  compiler     = "unknown";
  compilerPath = "unknown";

  pid_t my_pid = getppid();  // start with parent!
  while(1)
    {
      Process proc(my_pid);
      pid_t parent = proc.parent();
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
      
      my_pid = parent;
      if (found)
        continue;

      // If here then we have found our guess at the name of the compiler
      compiler     = name;
      compilerPath = proc.exe();
      proc.cmdline(linklineA);
      break;
    }
}

int main(int argc, char* argv[])
{
  std::string compiler;
  std::string compilerPath;
  Vstring     linklineA;

  extract_linker(compiler, compilerPath, linklineA);

  Json json;
  json.add("compiler",     compiler);
  json.add("compilerPath", compilerPath);
  json.add("link_line",    linklineA);
  json.fini();

  std::string jsonStr = json.result();
  fprintf(stdout,"%s\n",jsonStr.c_str());

  return 0;
}



