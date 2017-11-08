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

#define DATESZ    100
#define HERE fprintf(stderr,"%s:%d\n",__FILE__,__LINE__)
#define DEBUG0(fp,s)             if (xalt_tracing) fprintf((fp),s)
#define DEBUG1(fp,s,x1)          if (xalt_tracing) fprintf((fp),s,(x1))
#define DEBUG2(fp,s,x1,x2)       if (xalt_tracing) fprintf((fp),s,(x1),(x2))
#define DEBUG3(fp,s,x1,x2,x3)    if (xalt_tracing) fprintf((fp),s,(x1),(x2),(x3))
#define DEBUG4(fp,s,x1,x2,x3,x4) if (xalt_tracing) fprintf((fp),s,(x1),(x2),(x3),(x4))

static volatile double epoch()
{
  struct timeval tm;
  gettimeofday(&tm, 0);
  return tm.tv_sec + 1.0e-6*tm.tv_usec;
}

char* build_xalt_dir(const char* user_name, const char* home_dir);

int main(int argc, char* argv[])
{
  int   xalt_tracing     = 0;
  int   len;
  char* run_uuid = NULL;
  char* syshost  = NULL;
  char* p_dbg;
  char  c;

  p_dbg = getenv("XALT_TRACING");
  if (p_dbg)
    xalt_tracing = (strcmp(p_dbg,"yes") == 0) || (strcmp(p_dbg,"pkg") == 0);

  while ((c = getopt(argc,argv, "s:u:")) != -1)
    {
      switch (c)
	{
	case 's':
	  len     = strlen(optarg);
	  syshost = (char *) malloc(len+1);
	  memcpy(syshost, optarg, len+1);
	  break;
	  
	case 'u':
	  len      = strlen(optarg);
	  run_uuid = (char *) malloc(len+1);
	  memcpy(run_uuid, optarg, len+1);
	  break;
	}
    }
  
  if (! syshost)
    {
      DEBUG0(stderr,"Syshost is not set => quitting!\n");
      exit(0);
    }

  if (! run_uuid)
    {
      DEBUG0(stderr,"The run_uuid value is not set => quitting!\n");
      exit(0);
    }



  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

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
  int idx = 0;
  json_str[idx++] = '{';
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

  char* resultFn = NULL;
  
  if (strcasecmp(transmission, "file") == 0)
    {
      char  uuid_str[37];
      char  date_str[DATESZ];
      char* c_home = getenv("HOME");
      char* c_user = getenv("USER");

      if (c_home != NULL && c_user != NULL)
	{
          char* xalt_dir = build_xalt_dir(c_user, c_home);
	  time_t now = epoch();
	  strftime(date_str, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&now));
	  
	  build_uuid(uuid_str);
          //          1         2   |      3
          //0123456789 123456789 123456789 123456789 
	  //3de2c9d8-e857-4482-9aa4-4979620380fc	  
	  asprintf(&resultFn,"%spkg.%s.%s.%s.%s.json", xalt_dir, syshost,date_str,run_uuid,&uuid_str[24]);
	  DEBUG1(stderr,"resultFn: %s\n",resultFn);

	  free(xalt_dir);
	}
    }
  
  char* key = NULL;
  asprintf(&key,"key_%s",run_uuid);

  transmit(transmission, json_str, "pkg", key, syshost, resultFn);

  if (resultFn)
    free(resultFn);
  free(key);
  free(syshost);
  free(run_uuid);
  free(json_str);
  xalt_quotestring_free();

  return 0;
}

char* build_xalt_dir(const char* user_name, const char* home_dir)
{
  char* xalt_dir = NULL;
  
#ifdef HAVE_FILE_PREFIX
  asprintf(&xalt_dir,"%s/%s/",XALT_FILE_PREFIX, user_name);
#else
  asprintf(&xalt_dir,"%s/.xalt.d/",home_dir);
#endif
  return xalt_dir;
}

