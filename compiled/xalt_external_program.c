#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "transmit.h"
#include "xalt_quotestring.h"
#include "xalt_config.h"
#include "build_uuid.h"

char* build_xalt_dir(const char* user_name, const char* home_dir);

int main(int argc, char* argv[])
{
  int   len;
  char* run_uuid = NULL;
  char* syshost  = NULL;
  char  uuid_str[37];

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
  
  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

  // Count size of string. Adjust count for possible UTF8 chars and two quotes and either a colon or a comma:

  //{"a":"b","c":"d"}
  int i;
  int sz = 2;  // {}
  for (i = 1; i < argc; ++i)
    sz += 3*strlen(argv[i]) + 3;

  char* result = (char *)malloc(sz+1);

  const char *qs;
  int len;
  int idx = 0;
  result[idx++] = '{';
  for (i = 1; i < argc; i+=2)
    {
      qs            = xalt_quotestring(argv[i]);
      len           = strlen(qs);
      result[idx++] = '"';
      memcpy(&result[idx], qs, len);
      idx          += len;
      result[idx++] = '"';
      result[idx++] = ':';
      qs            = xalt_quotestring(argv[i+1]);
      len           = strlen(qs);
      result[idx++] = '"';
      memcpy(&result[idx], qs, len);
      idx          += len;
      result[idx++] = '"';
      result[idx++] = ',';
    }
  result[--idx] = '}';
  result[++idx] = '\0';

  printf("sz: %d, strlen(): %ld, result: %s\n",sz, strlen(result),result);


  char* resultFn = NULL;
  char* xalt_dir = NULL;
  
  if (strcasecmp(transmission, "file") == 0)
    {
      char* c_home = getenv("HOME");
      char* c_user = getenv("USER");

      if (c_home != NULL && c_user != NULL)
	{
	  xalt_dir = build_xalt_dir(c_user, c_home);
	  time     = 
	  

	  free(xalt_dir);
  


  


  if (syshost)
    free(syshost);

  if (uuid_str)
    free(uuid_str);

  if (resultFn)
    free(resultFn);

  free(result);
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

