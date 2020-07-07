#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "xalt_c_utils.h"

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
  utstring_new(dir);
  
  const char * xalt_file_prefix = getenv("XALT_FILE_PREFIX");
  if (xalt_file_prefix == NULL)
    xalt_file_prefix = XALT_FILE_PREFIX;

  if ((c_home != NULL) && (strcasecmp(xalt_file_prefix,"USE_HOME") == 0))
    {
      if (strcasecmp(transmission,"file_separate_dirs") == 0)
	asprintf(&resultDir,"%s/.xalt.d/%s/",c_home,kind);
      else
	asprintf(&resultDir,"%s/.xalt.d/",c_home);
    }
  else
    {
      long hashV = strtol(&uuid[29], 0, 16) % XALT_PRIME_NUMBER;
      if (strcasecmp(transmission,"file_separate_dirs") == 0)
	asprintf(&resultDir, "%s/%s/" XALT_PRIME_FMT, "/",xalt_file_prefix,kind,hashV);
      else
	asprintf(&resultDir, "%s/" XALT_PRIME_FMT, "/",xalt_file_prefix,hashV);
    }
}
