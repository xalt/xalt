#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ConfigParser.h"
#include "xalt_config.h"
#include "base64.h"
#include "xalt_fgets_alloc.h"
#include "xalt_utils.h"

void trim(char * s)
{
  char * p = s;
  int l = strlen(p);
  
  while(isspace(p[l - 1]))
    p[--l] = 0;
  while(* p && isspace(* p))
    ++p, --l;

  memmove(s, p, l + 1);
}

ConfigParser::ConfigParser(const char * fn)
{

  FILE* fp = xalt_file_open(fn);

  if (fp == NULL)
    {
      fprintf(stderr,"XALT Failure Cannot open %s\n", fn);
      exit(1);
    }

  char* buf  = NULL;
  size_t sz = 0;

  while (xalt_fgets_alloc(fp, &buf, &sz))
    {
      int    valueLen;
      char * key;
      char * value;
      char * p = strchr(buf,'=');
      if (p == NULL)
	continue;

      key = buf;
      value = p+1;
      *p = '\0';
      trim(key);
      trim(value);

      if      (strcmp(key,"host")   == 0) m_host   = value;
      else if (strcmp(key,"user")   == 0) m_user   = value;
      else if (strcmp(key,"db")     == 0) m_db     = value;
      else if (strcmp(key,"passwd") == 0)
        {
          unsigned char * passwd = base64_decode(value, strlen(value), &valueLen);
          m_passwd.assign(reinterpret_cast<char*>(passwd));
          free(passwd);
        }
    }
  free(buf); sz = 0; buf = NULL;
}
