#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "transmit.h"
#include "xalt_quotestring.h"
#include "xalt_config.h"
#include "xalt_c_utils.h"
#include "build_uuid.h"
#include "xalt_tmpdir.h"
#include "xalt_debug_macros.h"
#include "xalt_pkg_filter.h"
#include "crc.h"

#define DATESZ    100

const  char*           xalt_syshost();

void saveValue(char** name, const char* value)
{
  int   len = strlen(value);
  char* n   = (char *) malloc((len+1)*sizeof(char));
  memcpy(n,value,len+1);
  *name = n;
}


int main(int argc, char* argv[])
{
  char*       run_uuid	   = NULL;
  const char* my_host      = xalt_syshost();
  const char* user         = getenv("USER");
  char*       program      = NULL;
  char*       pkg_name     = NULL;
  char*       pkg_path     = NULL;
  char*       buf          = NULL;
  int         bufSz        = 0;
  int         xalt_tracing = 0;
  int         len;
  char*       p_dbg;
  char        c;

  if (!user)
    user = "UNKNOWN";

  p_dbg = getenv("XALT_TRACING");
  if (p_dbg)
    xalt_tracing = (strcmp(p_dbg,"yes") == 0) || (strcmp(p_dbg,"pkg") == 0);

  while ((c = getopt(argc,argv, "u:")) != -1)
    {
      switch (c)
	{
	case 'u':
	  len      = strlen(optarg);
	  run_uuid = (char *) XMALLOC(len+1);
	  memcpy(run_uuid, optarg, len+1);
	  break;
	}
    }
  
  if (! my_host)
    {
      DEBUG(stderr,"Syshost is not set => quitting!\n");
      exit(0);
    }

  if (! run_uuid)
    {
      DEBUG(stderr,"The run_uuid value is not set => quitting!\n");
      exit(0);
    }

  // Count size of string. Adjust count for possible UTF8 chars and two quotes and either a colon or a comma:

  //{"a":"b","c":"d"}
  int i;
  int sz = 2;  // {}
  for (i = optind; i < argc; ++i)
    sz += 3*strlen(argv[i]) + 3;

  char* json_str = (char *)XMALLOC(sz+1);

  if ((argc - optind) % 2 == 1)
    {
      DEBUG(stderr,"Odd number of parameter passed to %s => quiting!\n",argv[0]);
      exit(0);
    }


  const char *qs;
  strcpy(&json_str[0],"{\"crc\":\"0xFFFF\",");
  int idx = strlen(&json_str[0]);
  for (i = optind; i < argc; i+=2)
    {
      if      (strcmp(argv[i],"program")      == 0) saveValue(&program,  argv[i+1]);
      else if (strcmp(argv[i],"package_name") == 0) saveValue(&pkg_name, argv[i+1]);
      else if (strcmp(argv[i],"package_path") == 0) saveValue(&pkg_path, argv[i+1]);

      qs	      = xalt_quotestring(argv[i]);
      len	      = strlen(qs);
      json_str[idx++] = '"';
      memcpy(&json_str[idx], qs, len);
      idx	     += len;
      json_str[idx++] = '"';
      json_str[idx++] = ':';
      qs	      = xalt_quotestring(argv[i+1]);
      len	      = strlen(qs);
      json_str[idx++] = '"';
      memcpy(&json_str[idx], qs, len);
      idx	     += len;
      json_str[idx++] = '"';
      json_str[idx++] = ',';
    }
  json_str[--idx] = '}';
  json_str[++idx] = '\0';

  crc crcValue = crcFast(json_str,strlen(json_str));
  char crcStr[7];
  sprintf(&crcStr[0],"0x%04X",crcValue);
  memcpy(&json_str[8],crcStr,6);


  char* resultFn = NULL;
  
  char  uuid_str[37];
  build_uuid(uuid_str);
  build_uuid_cleanup();
  //          1         2   |      3
  //0123456789 123456789 123456789 123456789 
  //3de2c9d8-e857-4482-9aa4-4979620380fc	  

  char* date_str = getenv("XALT_DATE_TIME");
  
  char* xalt_tmpdir = create_xalt_tmpdir_str(run_uuid);
  asprintf(&resultFn,"pkg.%s.%s.%s.%s.%s.json", my_host, date_str, user,
                                               run_uuid, &uuid_str[24]);
  char* key = NULL;
  asprintf(&key,"pkg_%s_%s",run_uuid, &uuid_str[24]);

  // Form string with program:path:pkg_path
  idx              = 0;
  int len_program  = strlen(program);
  int len_pkg_path = strlen(pkg_path);
  int len_pkg_name = strlen(pkg_name);
    
  bufSz = len_program+1+5+len_pkg_path+1;
  buf   = (char *) malloc((bufSz+1)*sizeof(char));
  memcpy(&buf[idx], program, len_program); idx += len_program;
  buf[idx++] = ':';
  memcpy(&buf[idx], "path:", 5); idx += 5;
  memcpy(&buf[idx], pkg_path, len_pkg_path); idx += len_pkg_path;
  buf[idx] = '\0';
  xalt_parser path_status = keep_pkg(buf);
  
  // Form string with program:name:pkg_path
  idx              = 0;
  int newSz = len_program+1+5+len_pkg_name+1;
  if (newSz > bufSz)
    {
      free(buf);
      bufSz = newSz;
      buf   = (char *) malloc((bufSz+1)*sizeof(char));
    }
  memcpy(&buf[idx], program, len_program); idx += len_program;
  buf[idx++] = ':';
  memcpy(&buf[idx], "name:", 5); idx += 5;
  memcpy(&buf[idx], pkg_name, len_pkg_name); idx += len_pkg_name;
  buf[idx] = '\0';
  xalt_parser name_status = keep_pkg(buf);

  if (name_status != SKIP && path_status != SKIP) 
    transmit("file", json_str, "pkg", key, crcStr, my_host, xalt_tmpdir, resultFn, stderr);
  fflush(stderr);

  free(xalt_tmpdir);
  free(resultFn);
  free(key);
  free(run_uuid);
  free(json_str);
  if (program)  free(program);
  if (pkg_name) free(pkg_name);
  if (pkg_path) free(pkg_path);
  if (buf)      free(buf);

  xalt_quotestring_free();

  return 0;
}
