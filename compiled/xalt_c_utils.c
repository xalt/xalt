#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int isDirectory(const char *path)
{
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}
