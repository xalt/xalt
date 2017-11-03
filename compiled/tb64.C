#include "base64.h"
#include "compress_string.h"
#include "zstring.h"
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

  int    zslen = 2*lenJson+1;
  char * zs    = (char *) malloc(zslen);
  compress_string(jsonStr, Z_BEST_COMPRESSION, zs, &zslen);

  std::string s(zs,zslen);
  std::string js = decompress_string(s);

  printf("%ld %%%s%%\n",strlen(js.c_str()), js.c_str());


  b64 = base64_encode(zs, zslen, &b64len);

  zs = (char *) base64_decode(b64, b64len, &zslen);

  s.assign(zs, zslen);
  js = decompress_string(s);
  
  printf("%ld %%%s%%\n",js.size(), js.c_str());

  printf("js[16]: %s js[17]: %s\n", js.substr(16,1).c_str(), js.substr(17,1).c_str());

  return 0;
}
