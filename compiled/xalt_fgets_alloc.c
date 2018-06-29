#include <stdlib.h>
#include <string.h>
#include "xalt_fgets_alloc.h"

#define HERE fprintf(stderr,"%s:%d\n",__FILE__,__LINE__)

static const int SZ = 4096;

int xalt_fgets_alloc(FILE *fp, char ** buffer, size_t* sz)
{
  char*  buf    = *buffer;
  size_t size   = *sz;
  size_t used   = 0;
  size_t needed = SZ;

  do {
    if (size < needed)
      {
	size = needed;
	char *buf_new = (char *)realloc(buf, size);
	if (buf_new == NULL)
	  {
	    // Out-of-memory
	    free(buf);
	    *sz     = 0;
	    *buffer = NULL;
	    return 0;
	  }
	buf	= buf_new;
	*sz	= size;
	*buffer	= buf;
      }
    if (fgets(&buf[used], (int) (size - used), fp) == NULL) 
      // feof or ferror
      return 0;

    size_t length = strlen(&buf[used]);
    if (length + 1 != size - used) break;
    used += length;

    if (buf[used - 1] == '\n')
      break;
    else
      needed *= 2;
    
  } while (1);
  return 1;
}
