#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <regex.h>

#include "xalt_utils.h"
#include "xalt_config.h"
#include "xalt_regex.h"


int isDirectory(const char *path)
{
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

bool reject_env_name(const std::string& env_name)
{
  regex_t regex;
  char    msgbuf[100];
  bool    ret = (acceptEnvSz != 0);

  for (int i = 0; i < acceptEnvSz; ++i)
    {
      if (regcomp(&regex, acceptEnvA[i], 0))
        {
          fprintf(stderr,"Could not compile regex \"%s\"\n", acceptEnvA[i]);
          exit(1);
        }
      int iret = regexec(&regex, env_name.c_str(), 0, NULL, 0);
      if (iret == 0)
        {
          ret = false;
          break;
        }
      else if (iret != REG_NOMATCH)
        {
          regerror(iret, &regex, msgbuf, sizeof(msgbuf));
          fprintf(stderr, "acceptEnvA Regex match failed: %s\n", msgbuf);
          exit(1);
        }
      regfree(&regex);
    }

  if (ret)
    return true;
  
  for (int i = 0; i < ignoreEnvSz; ++i)
    {
      if (regcomp(&regex, ignoreEnvA[i], 0))
        {
          fprintf(stderr,"Could not compile regex \"%s\"\n", ignoreEnvA[i]);
          exit(1);
        }
      int iret = regexec(&regex, env_name.c_str(), 0, NULL, 0);
      if (iret == 0)
        return true;
      else if (iret != REG_NOMATCH)
        {
          regerror(iret, &regex, msgbuf, sizeof(msgbuf));
          fprintf(stderr, "ignoreEnvA Regex match failed: %s\n", msgbuf);
          exit(1);
        }
      regfree(&regex);
    }
  return false;
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

void build_xaltDir(std::string& xaltDir, std::string& userName, std::string& baseDir)
{
  
  #ifdef HAVE_FILE_PREFIX
    xaltDir.assign(XALT_FILE_PREFIX);
    xaltDir.append("/");
    xaltDir.append(userName);
    xaltDir.append("/");
  #else
    xaltDir.assign(baseDir);
    xaltDir.append("/.xalt.d/");
  #endif
}
