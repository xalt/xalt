#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include "zstring.h"
#include "xalt_c_utils.h"

#define BUFFSZ 32768
#define MAX(a,b) ((a) > (b) ? (a) : (b))

char* uncompress_string(const char* str, int len)
{
  z_stream zs;
  memset(&zs, 0, sizeof(zs));

  if (inflateInit(&zs) != Z_OK)
    {
      fprintf(stderr,"inflateInit failed while decompressing.\n");
      exit(1);
    }

  zs.next_in  = (Bytef*)str;
  zs.avail_in = len;

  int  ret, oldSz;
  char outbuffer[BUFFSZ];

  int   totalSz = 4*len;
  int   szOut   = 0;
  char* prev;
  char* out     = (char *) malloc(totalSz+1);

  do
    {
      zs.next_out  = (Bytef *) outbuffer;
      zs.avail_out = BUFFSZ;
      
      ret = inflate(&zs, 0);
      
      oldSz  = szOut;
      szOut  = zs.total_out;

      if ( szOut > totalSz)
	{
	  totalSz = MAX(2*totalSz,2*szOut);
	  prev = out;
	  out  = (char *) malloc(totalSz+1);
	  memcpy(out, prev, oldSz);
	  my_free(prev, strlen(prev));
	}
	    
      if (oldSz < szOut )
	{
	  memcpy(&out[oldSz], outbuffer, zs.total_out - oldSz);
	  out[szOut] = '\0';
	}
    }
  while(ret == Z_OK);

  inflateEnd(&zs);
  if (ret != Z_STREAM_END) 
    {
      // an error occurred that was not EOF
      fprintf(stderr,"Exception during zlib decompression: (%d) %s\n",ret, zs.msg);
      exit(1);
    }
  return out;
}
