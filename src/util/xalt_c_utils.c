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
#define HERE fprintf(stderr,"%s:%d\n",__FILE__,__LINE__)

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

const char* xalt_file_transmission_method()
{
  const char * xalt_file_prefix = xalt_getenv("XALT_FILE_PREFIX");
  if (xalt_file_prefix == NULL)
    xalt_file_prefix = XALT_FILE_PREFIX;
  if (strcasecmp(xalt_file_prefix,"USE_HOME") == 0)
    return "USE_HOME";
  return "XALT_FILE_PREFIX";
}


void build_resultDir(char **resultDir, const char* kind, const char* transmission, const char* uuid_str)
{

  char* c_home = xalt_getenv("HOME");

  const char * xalt_file_prefix = xalt_getenv("XALT_FILE_PREFIX");
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
        if (!XALT_PRIME_NUMBER)
        {
            if (strcasecmp(transmission,"file_separate_dirs") == 0)
                asprintf(resultDir, "%s/%s/",xalt_file_prefix, kind);
            else
                asprintf(resultDir, "%s/", xalt_file_prefix);
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
}

void build_resultFn(char** resultFn, const char* kind, double start, const char* syshost,
		    const char* uuid_str, const char* suffix)
{
  char* user = xalt_getenv("USER");
  char  dateStr[DATESZ];


  double frac = start - floor(start);
  time_t time = (time_t) start;
  strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));
  if (user != NULL)
    asprintf(resultFn, "%s.%s.%s_%04d.%s%s.%s.json", kind, syshost, dateStr, (int) (frac*10000.0),
             user, suffix, uuid_str);
  else
    {
      // if the user name was not successfully found, use the UID returned by
      // getuid().  getpwnam() and getpwuid() are not used since those many
      // require external services like LDAP and thus possibly delay the
      // program from starting.
      asprintf(resultFn, "%s.%s.%s_%04d.%d%s.%s.json", kind, syshost, dateStr, (int) (frac*10000.0),
               getuid(), suffix, uuid_str);
    }

}

static bool s_start_record = true;
void set_end_record()
{
  s_start_record = false;
}
void* xmalloc(size_t size, const char* fn, int lineNo)
{
  void* result = malloc(size);
  if (result != NULL)
    return result;
  // If here then malloc returned a NULL;
  fprintf(stdout, "xmalloc return a null called from %s:%d\n",fn,lineNo);
  exit(1);
}

extern char ** environ;

// Some programs, like bash, have their own getenv() implementation which
// gets added to the global symbol table before any provided by libraries.
// For some versions of bash, the replacement getenv() does not return the
// value when an LD_PRELOAD library calls it during initialization.
// In the following code, getenv() is tried first, and if tha fails to get
// the value, the environ table is scanned.
char *xalt_getenv(const char *key)
{
  char* value = getenv(key);
  if (value != NULL)
    return value;
  else
    {
      if (environ == NULL || key[0] == '\0')
        return NULL;
      size_t len = strlen (key);
      for (char ** ep = environ; *ep != NULL; ++ep)
        {
          if (key[0] == (*ep)[0]
              && strncmp (key, *ep, len) == 0 && (*ep)[len] == '=')
            return *ep + len + 1;
        }
    }
  return NULL;
}

void my_free(void *ptr, int sz)
{
  if (s_start_record && ptr != NULL)
    {
      memset(ptr, '\0', sz);
      free(ptr);
    }
}
