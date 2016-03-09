#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FGETS_ALLOC_N 128

char* fgets_alloc(FILE *istream)
{
  char* buf = NULL;
  size_t size = 0;
  size_t used = 0;
  do {
    size += FGETS_ALLOC_N;
    char *buf_new = realloc(buf, size);
    if (buf_new == NULL) {
      // Out-of-memory
      free(buf);
      return NULL;
    }
    buf = buf_new;
    if (fgets(&buf[used], (int) (size - used), istream) == NULL) {
      // feof or ferror
      if (used == 0 || ferror(istream)) {
        free(buf);
        buf = NULL;
      }
      return buf;
    }
    size_t length = strlen(&buf[used]);
    if (length + 1 != size - used) break;
    used += length;
  } while (buf[used - 1] != '\n');
  return buf;
}



const char * xalt_syshost()
{
  const char* fname = "/etc/syshost.xalt";
  FILE *fp = fopen(fname,"r");
  if (! fp)
    fprintf(stderr,"unable to open %s\n",fname);

  return fgets_alloc(fp);
}

int main()
{
  printf("syshost: %s\n",xalt_syshost());
}
