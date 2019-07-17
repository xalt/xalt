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
#include "xalt_regex.h"
#include "Options.h"

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


#define BITS_IN_int    (sizeof (int)*CHAR_BIT)
#define THREE_QUARTERS ((int) ((BITS_IN_int * 3) / 4))
#define ONE_EIGHTH     ((int) (BITS_IN_int / 8))
#define HIGH_BITS      ( ~ ((unsigned int) (~0) >> ONE_EIGHTH ))

unsigned int hashStr(const char * string)
{
  unsigned int hash_value, i;
  for ( hash_value = 0; *string; ++string)
    {
      hash_value = (hash_value << ONE_EIGHTH) + *string;
      if ( (i = hash_value & HIGH_BITS ) != 0)
        hash_value = (hash_value ^ (i >> THREE_QUARTERS )) & ~HIGH_BITS;
    }
  return hash_value;
}


void build_resultDir(std::string& resultDir, const char *kind, const char* transmission)
{
  char* c_home = getenv("HOME");
  char* c_user = getenv("USER");

  if (c_home != NULL && c_user != NULL )
    {
      const char * xalt_file_prefix = getenv("XALT_FILE_PREFIX");
      if (xalt_file_prefix == NULL)
        xalt_file_prefix = XALT_FILE_PREFIX;
      if (strcasecmp(xalt_file_prefix,"USE_HOME") == 0)
        { 
          resultDir.assign(c_home);
          resultDir.append("/.xalt.d/");
          if (strcasecmp(transmission,"file_separate_dirs") == 0)
            {
              resultDir.append(kind);
              resultDir.append("/");
            }
        }
      else
        {
          resultDir.assign(xalt_file_prefix);
          resultDir.append("/");
          if (strcasecmp(transmission,"file_separate_dirs") == 0)
            {
              resultDir.append(kind);
              resultDir.append("/");
            }
          char * hashDir = NULL;
          asprintf(&hashDir, XALT_PRIME_FMT "/",hashStr(c_user) % XALT_PRIME_NUMBER);
          resultDir.append(hashDir);
          free(hashDir);
        }
    }
}

void build_resultFn(std::string& resultFn, double start, const char* syshost, const char* uuid, const char *kind,
                    const char* suffix)
{
  char dateStr[DATESZ];
  char* c_home = getenv("HOME");
  char* c_user = getenv("USER");
  
  if (c_home != NULL && c_user != NULL )
    {
      double frac  = start - floor(start);
      time_t  time = start;
      strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));

      std::ostringstream sstream;
      sstream << kind   << "." << syshost << "." << dateStr 
              << "_" << std::setfill('0') << std::setw(4) << (int) (frac*10000.0)
              << "." << c_user << suffix << "."    << uuid  << ".json";

      resultFn = sstream.str();
    }
}
