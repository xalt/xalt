#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "xalt_utils.h"
#include "xalt_config.h"

int isDirectory(const char *path)
{
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

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

FILE* xalt_file_open(const char* name)
{
  FILE*       fp           = NULL;
  const char* xalt_etc_dir = getenv("XALT_ETC_DIR");
  std::string dirNm;
  std::string fn;

  fp = fopen(name,"r");
  if (fp)
    return fp;

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

      fn.assign(dirNm);
      fn += "/";
      fn += name;
      fp  = fopen(fn.c_str(), "r");
      if (fp)
        break;

      start = ++p;
    } 
  return fp;
}


FILE* xalt_json_file_open(const char* name)
{
  static const char *extA[]       = {".json", ".old.json"};
  static int         nExt         = sizeof(extA)/sizeof(extA[0]);
  const char*        xalt_etc_dir = getenv("XALT_ETC_DIR");
  std::string        dirNm;
  std::string        fn;

  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  FILE*       fp    = NULL;
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
