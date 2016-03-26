#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ConfigParser.h"
#include "xalt_config.h"
#include "base64.h"
#include "fgets_alloc.h"

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

  const char * xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  std::string confFn(xalt_etc_dir);
  confFn.append("/");
  confFn.append(fn);
  char * buf;
  

  FILE* fp = fopen(confFn.c_str(),"r");
  if (fp == NULL)
    {
      fprintf(stderr,"XALT Failure Cannot open %s\n", confFn.c_str());
      exit(1);
    }

  while ((buf = fgets_alloc(fp)) != NULL)
    {
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
      else if (strcmp(key,"passwd") == 0) m_passwd = base64_decode(value);
      else if (strcmp(key,"db")     == 0) m_db     = value;
    }
}
