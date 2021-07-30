#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include "xalt_types.h"
#include "process.h"
#include "buildJson.h"
#include "utarray.h"

static const char* blank0      = "";
static const char* comma       = ",";

void extract_linker(std::string& compiler, std::string& compilerPath, UT_array** linklineA)
{

  //--------------------------------------------------
  // Walk process tree to find compiler name and path

  std::string ignorePrgA[] = {"ld", "ld.gold", "collect2","bash","Python", "python", "sh",
                              "x86_64-linux-gn", "x86_64-linux-gnu-ld", "x86_64-linux-gnu-ld.bfd",
                              "x86_64-linux-gnu-ld.gold" };
  int         ignorePrgSz  = sizeof(ignorePrgA)/sizeof(ignorePrgA[0]);

  compiler               = "unknown";
  compilerPath           = "unknown";
  std::string parentProg = "unknown";
  process_t proc;

  pid_t my_pid = getppid();  // start with parent!
  init_proc(&proc);
  while(1)
    {
      build_proc(&proc, my_pid);
      fflush(stderr);

      if (proc.m_pid < 2)
        break;

      std::string name = utstring_body(proc.m_name);
      bool ignore = false;
      for (int i = 0; i < ignorePrgSz; ++i)
        {
          if (name == ignorePrgA[i])
            {
              ignore = true;
              break;
            }
        }

      my_pid = proc.m_parent;
      if (ignore)
        continue;

      // If here then we have found our guess at the name of the compiler
      compiler     = name;
      compilerPath = utstring_body(proc.m_exe);
      proc_cmdline(&proc, linklineA);

      my_pid = proc.m_parent;
      build_proc(&proc, my_pid);

      parentProg = utstring_body(proc.m_name);
      if (parentProg == "rustc")
        compilerPath = utstring_body(proc.m_exe);
      break;
    }
  free_proc(&proc);
}

int main(int argc, char* argv[])
{
  std::string compiler;
  std::string compilerPath;
  UT_array*   linklineA;
  utarray_new(linklineA,&ut_str_icd);

  extract_linker(compiler, compilerPath, &linklineA);

  const char* my_sep = blank0;
  char*       jsonStr;
  Json_t      json;
  json_init(Json_TABLE, &json);

  json_add_char_str(&json, my_sep, "compiler",     compiler.c_str());      my_sep = comma;
  json_add_char_str(&json, my_sep, "compilerPath", compilerPath.c_str());
  json_add_utarray( &json, my_sep, "link_line",    linklineA);
  json_fini(&json, &jsonStr);

  fprintf(stdout,"%s\n",jsonStr);

  return 0;
}
