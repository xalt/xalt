#define _GNU_SOURCE
#include "xalt_config.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "xalt_c_utils.h"

#define DATESZ 100

int isDirectory(const char *path)
{
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

int mkpath(char* file_path, mode_t mode)
{
  char* p;
  for (p=strchr(file_path+1, '/'); p; p=strchr(p+1, '/'))
    {
      *p='\0';
      if (mkdir(file_path, mode)==-1)
	{
	  if (errno != EEXIST)
	    {
	      *p='/';
	      return -1;
	    }
	}
      *p='/';
    }
  return 0;
}

void build_resultDir(char **resultDir, const char* kind, const char* transmission, const char* uuid_str)
{

  char* c_home = getenv("HOME");
  
  const char * xalt_file_prefix = getenv("XALT_FILE_PREFIX");
  if (xalt_file_prefix == NULL)
    xalt_file_prefix = XALT_FILE_PREFIX;

  if ((c_home != NULL) && (strcasecmp(xalt_file_prefix,"USE_HOME") == 0))
    {
      if (strcasecmp(transmission,"file_separate_dirs") == 0)
	asprintf(resultDir,"%s/.xalt.d/%s/",c_home,kind);
      else
	asprintf(resultDir,"%s/.xalt.d/",c_home);
    }
  else
    {
      int hashV = (int) strtol(&uuid_str[29], 0, 16) % XALT_PRIME_NUMBER;
      if (strcasecmp(transmission,"file_separate_dirs") == 0)
	asprintf(resultDir, "%s/%s/" XALT_PRIME_FMT "/",xalt_file_prefix, kind, hashV);
      else
	asprintf(resultDir, "%s/" XALT_PRIME_FMT "/",xalt_file_prefix,hashV);
    }
}

void build_resultFn(char** resultFn, const char* kind, double start, const char* syshost,
		    const char* uuid_str, const char* suffix)
{
  char* home = getenv("HOME");
  char* user = getenv("USER");
  char  dateStr[DATESZ];

  if (home != NULL && user != NULL)
    {
      double frac = start - floor(start);
      time_t time = (time_t) start;
      strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));
      
      asprintf(resultFn, "%s.%s.%s_%04d.%s%s.%s.json", kind, syshost, dateStr, (int) (frac*10000.0),
	       user, suffix, uuid_str);
    }
}

static bool s_end_record = false;
void set_end_record()
{
  s_end_record = true;
}
void my_free(void *ptr)
{
  if (s_end_record)
    free(ptr);
}
