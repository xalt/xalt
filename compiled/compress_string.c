#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include "zstring.h"

char* compress_string(const char* str, int* lenOut)
{
  uLong ucompSize = strlen(str)+1;
  uLong compSize  = compressBound(ucompSize);

  out = (char*) malloc(compSize);

  compress((Bytef *) out, &compSize, (Bytef *) str, ucompSize);
  *lenOut = (int) compSize;

  return out;
}

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

  int  ret;
  char outbuffer[32768];

  int   szOut = 0;
  char* out   = NULL;
  char* prev;

  do
    {
      zs.next_out = (Bytef *) outbuffer;
      zs.avail_out = sizeof(outbuffer);
      
      ret = inflate(&zs, 0);
      
      oldSz  = szOut;
      szOut += zs.total_out;

      prev   = out;
      out    = (char *) malloc(szOut);
      memcpy(out, prev, oldSz);
      memcpy(&out[oldSz], outbuffer, zs.total_out);
    }
  while(ret == Z_OK);

  inflateEnd(&zs);
  if (ret != Z_STREAM_END) 
    {
      // an error occurred that was not EOF
      fprintf(stderr,"Exception during zlib decompression: (%d) %s\n",ret, zs.msg);
      exit(1);
    }
  return outstring;
}
