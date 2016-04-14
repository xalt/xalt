#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 128

#include "iniparser.h"
#include "b64/cdecode.h"

char* decode(const char* input)
{
  char *output = (char *) malloc(SIZE);
  char *c      = output;
  int  cnt     = 0;
  base64_decodestate s;
  
  /* Start Decoding */
  base64_init_decodestate(&s);
  cnt  = base64_decode_block(input, strlen(input), c, &s);
  c   += cnt;
  *c   = '\0';

  return output;
}


int main(int argc, char* argv[])
{
  if (argc < 2)
    {
      printf("usage %s config_file\n",argv[0]);
      return 0;
    }

  dictionary  *   ini ;
  char*           s;
  char*           passwd;

  ini = iniparser_load(argv[1]);

  if ( ini == NULL)
    {
      fprintf(stderr, "cannot parse file: %s\n", argv[1]);
      return -1;
    }

  s = iniparser_getstring(ini, "MYSQL:host", NULL);
  printf("host: %s\n", s);
  s = iniparser_getstring(ini, "MYSQL:user", NULL);
  printf("user: %s\n", s);
  s = iniparser_getstring(ini, "MYSQL:db", NULL);
  printf("db: %s\n", s);
  s = iniparser_getstring(ini, "MYSQL:passwd", NULL);
  passwd = decode(s);
  printf("passwd: %s\n", passwd);
	
  
  return 0;
}
