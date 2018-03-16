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
