#define  _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>
#include "transmit.h"
#include "compress_string.h"
#include "base64.h"
#include "xalt_config.h"
#include "xalt_c_utils.h"
#include "xalt_base_types.h"

const int syslog_msg_sz = SYSLOG_MSG_SZ;

void transmit(const char* transmission, const char* jsonStr, const char* kind, const char* key,
              const char* syshost, const char* resultFn)
{
  char * cmdline;
  char * p_dbg        = getenv("XALT_TRACING");
  int    xalt_tracing = (p_dbg && (strcmp(p_dbg,"yes") == 0 || strcmp(p_dbg,"run") == 0 ));

  if (strcasecmp(transmission, "file") == 0)
    {
      //*********************************************************************
      // Build file name for xalt json record.  It is only used when the
      // file transmission style is used;
      if (resultFn == NULL)
        return;

      int   lenFn   = strlen(resultFn);
      char* dirname = malloc(lenFn+1);
      memcpy(dirname, resultFn, lenFn+1);

      char* p       = strrchr(dirname,'/');
      *p = '\0';
      if ( ! isDirectory(dirname))
        mkdir(dirname, 0700);
      free(dirname);

      FILE* fp = fopen(resultFn,"w");
      if (fp == NULL && xalt_tracing)
        fprintf(stderr,"  Unable to open: %s\n", resultFn);
      else
        {
          fprintf(fp, "%s\n", jsonStr);
          fclose(fp);
          DEBUG2(stderr,"  Wrote json %s file : %s\n",kind, resultFn);
        }
    }
  else if (strcasecmp(transmission, "syslogv1") == 0)
    {
      int   lenJson = strlen(jsonStr);
      int   zslen   = 2*lenJson+1;
      char* zs      = malloc(zslen);
      compress_string(jsonStr, Z_BEST_COMPRESSION, zs, &zslen);
      int   b64len;
      char* b64     = base64_encode(zs, zslen, &b64len);
      
      asprintf(&cmdline, "%s -t XALT_LOGGING_%s \"%s:%s\"\n",LOGGER, syshost, kind, b64);
      system(cmdline);
      free(zs);
      free(b64);
      free(cmdline);
      
    }
  else if (strcasecmp(transmission, "syslog") == 0)
    {
      int   lenJson = strlen(jsonStr);
      char* zs      = malloc(2*lenJson+1);
      int   zslen;
      compress_string(jsonStr, Z_BEST_COMPRESSION, zs, &zslen);
      int   sz;
      char* b64     = base64_encode(zs, zslen, &sz);
      
      int   blkSz   = (sz < syslog_msg_sz) ? sz : syslog_msg_sz;
      int   nBlks   = (sz -  1)/blkSz + 1;
      int   istrt   = 0;
      int   iend    = blkSz;
      int   i;

      for (i = 0; i < nBlks; i++)
        {
          asprintf(&cmdline, "%s XALT_LOGGER_%s V:2 kind:%s idx:%d nb:%d syshost:%s key:%s value:%.*s\n",
                   LOGGER, syshost, kind, i, nBlks, syshost, key, iend-istrt, &b64[istrt]);
          system(cmdline);
          free(cmdline);
          istrt = iend;
          iend  = istrt + blkSz;
          if (iend > sz)
            iend = sz;
        }
      free(b64);
    }
}
