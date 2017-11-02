#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include "compress_string.h"

int compress_string(const char* str, int compressionlevel, char* out, int* lenOut)
{
  z_stream zs;                        // z_stream is zlib's control structure
  memset(&zs, 0, sizeof(zs));

  zs.zalloc   = Z_NULL;
  zs.zfree    = Z_NULL;
  zs.opaque   = Z_NULL;
  zs.next_in  = (Bytef*)str;
  zs.avail_in = strlen(str)+1;          // set the z_stream's input

  zs.avail_out = *lenOut;               // on input lenOut is the space avail in out
  zs.next_out  = (Bytef *) out;

  // The actual compression work.
  if (deflateInit(&zs, compressionlevel) != Z_OK)
    {
      fprintf(stderr, "Unable to set compression level with deflateInit\n");
      exit(1);
    }
      
  deflate(&zs, Z_FINISH);
  deflateEnd(&zs);
  *lenOut = (int) zs.total_out;
  return 0;
}
