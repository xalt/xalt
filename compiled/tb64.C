#include "base64.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>
#include "zstring.h"
#include <string>

int main(int argc, char* argv[])
{
  const char* jsonStr = "[ \"a\", \"b\", \"c\" ]";
  int lenJson = strlen(jsonStr);
  int b64len;
  char* b64   = base64_encode(jsonStr, lenJson, &b64len);

  int lenJ2;
  unsigned char* jstr = base64_decode(b64, b64len, &lenJ2);

  free(b64);

  printf("%d, %d, %s\n",lenJson, lenJ2, jstr);

  int zslen;

  char * zs    = compress_string(jsonStr, &zslen);

  char * js = uncompress_string(zs, zslen);


  printf("%ld %%%s%%\n",strlen(js), js);

  free(js);

  b64 = base64_encode(zs, zslen, &b64len);

  zs = (char *) base64_decode(b64, b64len, &zslen);

  js = uncompress_string(zs,zslen);
  
  printf("%ld %%%s%%\n",strlen(js), js);

  free(js);
  return 0;
}
