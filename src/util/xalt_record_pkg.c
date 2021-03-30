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
#include "build_uuid.h"
#include "xalt_tmpdir.h"
#include "crc.h"

#define DATESZ    100
#define HERE fprintf(stderr,"%s:%d\n",__FILE__,__LINE__)
#define DEBUG0(fp,s)             if (xalt_tracing) fprintf((fp),s)
#define DEBUG1(fp,s,x1)          if (xalt_tracing) fprintf((fp),s,(x1))
#define DEBUG2(fp,s,x1,x2)       if (xalt_tracing) fprintf((fp),s,(x1),(x2))
#define DEBUG3(fp,s,x1,x2,x3)    if (xalt_tracing) fprintf((fp),s,(x1),(x2),(x3))
#define DEBUG4(fp,s,x1,x2,x3,x4) if (xalt_tracing) fprintf((fp),s,(x1),(x2),(x3),(x4))

const  char*           xalt_syshost();


int main(int argc, char* argv[])
{
  char*       run_uuid	   = NULL;
  const char* my_host      = xalt_syshost();
  const char* user         = getenv("USER");
  int         xalt_tracing = 0;
  int         len;
  char*       p_dbg;
  char        c;

  if (!user)
    user = "UNKNOWN";

  p_dbg = getenv("XALT_TRACING");
  if (p_dbg)
    xalt_tracing = (strcmp(p_dbg,"yes") == 0) || (strcmp(p_dbg,"pkg") == 0);

  while ((c = getopt(argc,argv, "s:u:")) != -1)
    {
      switch (c)
	{
	case 'u':
	  len      = strlen(optarg);
	  run_uuid = (char *) malloc(len+1);
	  memcpy(run_uuid, optarg, len+1);
	  break;
	}
    }
  
  if (! my_host)
    {
      DEBUG0(stderr,"Syshost is not set => quitting!\n");
      exit(0);
    }

  if (! run_uuid)
    {
      DEBUG0(stderr,"The run_uuid value is not set => quitting!\n");
      exit(0);
    }

  // Count size of string. Adjust count for possible UTF8 chars and two quotes and either a colon or a comma:

  //{"a":"b","c":"d"}
  int i;
  int sz = 2;  // {}
  for (i = optind; i < argc; ++i)
    sz += 3*strlen(argv[i]) + 3;

  char* json_str = (char *)malloc(sz+1);

  if ((argc - optind) % 2 == 1)
    {
      DEBUG1(stderr,"Odd number of parameter passed to %s => quiting!\n",argv[0]);
      exit(0);
    }


  const char *qs;
  strcpy(&json_str[0],"{\"crc\":\"0xFFFF\",");
  int idx = strlen(&json_str[0])
  for (i = optind; i < argc; i+=2)
    {
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

  crcInit();
  crc crcValue = crcFast(jsonStr,strlen(jsonStr));
  char crcStr[7];
  sprintf(&crcStr[0],"0x%04X",crcValue);
  memcpy(&jsonStr[8],crcStr,6);


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

  transmit("file", json_str, "pkg", key, my_host, xalt_tmpdir, resultFn, stderr);
  fflush(stderr);

  free(xalt_tmpdir);
  free(resultFn);
  free(key);
  free(run_uuid);
  free(json_str);
  xalt_quotestring_free();

  return 0;
}
