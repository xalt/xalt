#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "zstring.h"

/** Compress a STL string using zlib with given compression level and return
  * the binary data. */
std::string compress_string(const std::string& str, int compressionlevel )
{
  z_stream zs;                        // z_stream is zlib's control structure
  memset(&zs, 0, sizeof(zs));

  if (deflateInit(&zs, compressionlevel) != Z_OK)
    {
      fprintf(stderr, "Unable to set compression level with deflateInit\n");
      exit(1);
    }
      
  zs.next_in = (Bytef*)str.data();
  zs.avail_in = str.size();           // set the z_stream's input

  int ret;
  char outbuffer[32768];
  std::string outstring;

  // retrieve the compressed bytes blockwise
  do
    {
      zs.next_out  = reinterpret_cast<Bytef*>(outbuffer);
      zs.avail_out = sizeof(outbuffer);

      ret = deflate(&zs, Z_FINISH);

      if (outstring.size() < zs.total_out)
        // append the block to the output string
        outstring.append(outbuffer,
                         zs.total_out - outstring.size());
    } 
  while (ret == Z_OK);

  deflateEnd(&zs);

  if (ret != Z_STREAM_END)
    {  
      // an error occurred that was not EOF
      fprintf(stderr,"Exception during zlib compression: (%d) %s\n",ret, zs.msg);
      exit(1);
    }
  return outstring;
}

/** Decompress an STL string using zlib and return the original data. */
std::string decompress_string(const std::string& str)
{
  z_stream zs;                        // z_stream is zlib's control structure
  memset(&zs, 0, sizeof(zs));

  if (inflateInit(&zs) != Z_OK)
    {
      fprintf(stderr,"inflateInit failed while decompressing.\n");
      exit(1);
    }

  zs.next_in  = (Bytef*)str.data();
  zs.avail_in = str.size();

  int ret;
  char outbuffer[32768];
  std::string outstring;

  // get the decompressed bytes blockwise using repeated calls to inflate
  do
    {
      zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
      zs.avail_out = sizeof(outbuffer);
      
      ret = inflate(&zs, 0);
      
      if (outstring.size() < zs.total_out) 
        outstring.append(outbuffer, zs.total_out - outstring.size());

    }
  while (ret == Z_OK);

  inflateEnd(&zs);

  if (ret != Z_STREAM_END) 
    {
      // an error occurred that was not EOF
      fprintf(stderr,"Exception during zlib decompression: (%d) %s\n",ret, zs.msg);
      exit(1);
    }
  return outstring;
}
