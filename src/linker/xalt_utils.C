#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <regex.h>
#include <time.h>

#include "xalt_utils.h"
#include "xalt_config.h"

#define DATESZ 100

bool path2module(const char* path, Table& rmapT, char* module_name, int module_name_sz)
{
  std::string p(path);
  bool found = false;

  while(1)
    {
      std::string::size_type idx = p.rfind('/');
      if (idx == std::string::npos)
        break;

      p.erase(idx, std::string::npos);
      Table::const_iterator got = rmapT.find(p);
      if (got != rmapT.end())
        {
          found   = true;
          int len = (*got).second.size();
          if (len > (module_name_sz - 1))
            len = module_name_sz - 1;
          memcpy(module_name, (*got).second.c_str(),len);
          module_name[len] = '\0';
          break;
        }
    }
  return found;
}


FILE* xalt_json_file_open(std::string& rmapD, const char* name)
{
  FILE*              fp     = NULL;
  static const char *extA[] = {".json", ".old.json"};
  static int         nExt   = sizeof(extA)/sizeof(extA[0]);
  std::string        dirNm;
  std::string        fn;

  // First look in rmapD if it has value.
  if (rmapD.size() > 0)
    {
      for (int i = 0; i < nExt; ++i)
        {
          fn.assign(rmapD);
          fn += "/";
          fn += name;
          fn += extA[i];
          fp  = fopen(fn.c_str(), "r");
          if (fp)
            return fp;
        }
    }

  // Otherwise search directories in XALT_ETC_DIR

  const char* xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  const char* start = xalt_etc_dir;
  bool        done  = false;
  while(!done)
    {
      char * p = strchr((char *) start,':');
      if (p)
        dirNm.assign(start, p - start);  
      else
        {
          dirNm.assign(start);
          done = true;
        }

      for (int i = 0; i < nExt; ++i)
        {
          fn.assign(dirNm);
          fn += "/";
          fn += name;
          fn += extA[i];
          fp  = fopen(fn.c_str(), "r");
          if (fp)
            break;
        }
      start = ++p;
    } 
  return fp;
}
