#include <stdio.h>
#include <string.h>
#include "link_submission.h"
#include "xalt_fgets_alloc.h"
#include "xalt_config.h"
#include "capture.h"

void parseLDTrace(const char* xaltobj, const char* linkfileFn, std::vector<Libpair>& libA)
{
  std::string path;
  char* buf = NULL;
  size_t sz = 0;
  
  Set set;

  FILE* fp = fopen(linkfileFn,"r");

  while(xalt_fgets_alloc(fp, &buf, &sz))
    {
      // Remove lines with ':'
      if (strchr(buf,':'))
        continue;

      // Remove the line with the xalt.o file
      if (strstr(buf, xaltobj))
        continue;

      // Remove any lines that start with /tmp/
      // This will ignore files like /tmp/ccT33qQt.o
      char * p = strstr(buf,"/tmp/");
      if (buf == p)
        continue;

      if (*buf == '-')
        {
          // Capture the library name in the parens:
          // -lgcc_s (/usr/lib/gcc/x86_64-linux-gnu/4.8/libgcc_s.so)
          char * start = strchr(buf,'(');
          char * p     = strchr(start+1,')');
          if ( start == NULL || p == NULL)
            continue;

          path.assign(start+1, p-1);
          char* my_realpath = canonicalize_file_name(path.c_str());
          path.assign(my_realpath);
          set.insert(path);
          free(my_realpath);
        }

      // Save everything else (and get rid of the trailing newline!)
      path.assign(buf, strlen(buf)-1);
      char* my_realpath = canonicalize_file_name(path.c_str());
      path.assign(my_realpath);
      set.insert(path);
      free(my_realpath);
    }

  free(buf);
  buf = NULL;

  for(auto it = set.begin(); it != set.end(); ++it)
    {
      std::vector<std::string> result;
      std::string cmd = SHA1SUM " " + *it;
      capture(cmd,result);
      std::string sha1 = result[0].substr(0, result[0].find(" "));

      Libpair libpair(*it, sha1);
      libA.push_back(libpair);
    }
}

void readFunctionList(const char* fn, Set& funcSet)
{
  char*  buf = NULL;
  size_t sz  = 0;
  
  FILE* fp = fopen(fn,"r");
  std::string funcName;

  while(xalt_fgets_alloc(fp, &buf, &sz))
    {
      funcName.assign(buf, strlen(buf) - 1);
      funcSet.insert(funcName);
    }
}
