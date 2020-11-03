#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include "zstring.h"
#include "xalt_c_utils.h"

#define BUFFSZ 32768
#define MAX(a,b) ((a) > (b) ? (a) : (b))

char* compress_string(const char* str, int* lenOut)
{
  z_stream zs;
  memset(&zs, 0, sizeof(zs));
  if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK)
    {
      fprintf(stderr, "Unable to set compression level with deflateInit\n");
      exit(1);
    }

  zs.next_in  = (Bytef *) str;
  zs.avail_in = strlen(str);

  int  ret, oldSz;
  char outbuffer[BUFFSZ];

  
  int  szOut   = 0;
  int  totalSz = zs.avail_in/2;
  char *prev;
  char *out    = (char *) malloc(totalSz+1);

  do
    {
      zs.next_out  = (Bytef *) outbuffer;
      zs.avail_out = BUFFSZ;

      ret = deflate(&zs, Z_FINISH);

      oldSz = szOut;
      szOut = zs.total_out;

      if (szOut > totalSz)
	{
	  totalSz = MAX(2*totalSz, 2*szOut);
	  prev    = out;
	  out     = (char *)malloc(totalSz+1);
	  memcpy(out, prev, oldSz);
	  my_free(prev,oldSz);
	}
      if (oldSz < szOut)
	{
	  memcpy(&out[oldSz], outbuffer, zs.total_out - oldSz);
	  out[szOut] = '\0';
	}
    }
  while(ret == Z_OK);
  *lenOut = szOut;
  return out;
}


