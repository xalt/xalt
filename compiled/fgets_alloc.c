#include <stdlib.h>
#include <string.h>
#include "fgets_alloc.h"

const int SZ = 100;

char* fgets_alloc(FILE *fp)
{
  char* buf = NULL;
  size_t size = 0;
  size_t used = 0;
  do {
    size += SZ;
    char *buf_new = (char *)realloc(buf, size);
    if (buf_new == NULL) {
      // Out-of-memory
      free(buf);
      return NULL;
    }
    buf = buf_new;
    if (fgets(&buf[used], (int) (size - used), fp) == NULL) {
      // feof or ferror
      if (used == 0 || ferror(fp)) {
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
