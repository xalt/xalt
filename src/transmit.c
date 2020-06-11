#define  _GNU_SOURCE
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <zlib.h>
#include <dlfcn.h>
#include "transmit.h"
#include "zstring.h"
#include "base64.h"
#include "xalt_config.h"
#include "xalt_c_utils.h"
#include "xalt_base_types.h"

const int syslog_msg_sz = SYSLOG_MSG_SZ;

struct response {
  char *memory;
  size_t size;
};

/* Callback to handle the HTTP response */
static size_t _write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct response *mem = (struct response *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (ptr == NULL) {
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

void transmit(const char* transmission, const char* jsonStr, const char* kind, const char* key,
              const char* syshost, char* resultDir, const char* resultFn)
{
  char * cmdline = NULL;
  char * p_dbg        = getenv("XALT_TRACING");
  int    xalt_tracing = (p_dbg && (strcmp(p_dbg,"yes")  == 0 ||
				   strcmp(p_dbg,"run")  == 0 ));

  if (strcasecmp(transmission,"directdb") == 0)
    {
      DEBUG0(stderr,"  Direct to DB transmission is NOT supported!\n");
      return;
    }


  if ((strcasecmp(transmission,"file")      != 0 ) &&
      (strcasecmp(transmission,"syslog")    != 0 ) && 
      (strcasecmp(transmission,"logger")    != 0 ) && 
      (strcasecmp(transmission,"none")      != 0 ) && 
      (strcasecmp(transmission,"syslogv1")  != 0 ) &&
      (strcasecmp(transmission,"curl")      != 0 ))
    transmission = "file";

  if (strcasecmp(transmission, "file") == 0 || strcasecmp(transmission, "file_separate_dirs") == 0 )
    {
      if (resultFn == NULL)
	{
	  DEBUG0(stderr,"    resultFn is NULL, $HOME or $USER might be undefined -> No XALT output\n");
	  return;
	}

      int err = mkpath(resultDir, 0700);
      if (err)
	{
	  if (xalt_tracing)
	    {
	      perror("Error: ");
	      fprintf(stderr,"    unable to mkpath(%s) -> No XALT output\n", resultDir);
	    }
	  return;
	}

      char* tmpFn = NULL;
      asprintf(&tmpFn, "%s.%s.new",resultDir, resultFn);
      char* fn = NULL;
      asprintf(&fn, "%s%s",resultDir, resultFn);

      FILE* fp = fopen(tmpFn,"w");
      if (fp == NULL && xalt_tracing)
	fprintf(stderr,"    Unable to open: %s -> No XALT output\n", fn);
      else
        {
          fprintf(fp, "%s\n", jsonStr);
          fclose(fp);
          rename(tmpFn, fn);
          DEBUG2(stderr,"    Wrote json %s file : %s\n",kind, fn);
        }
      free(tmpFn);
      free(fn);
    }
  else if (strcasecmp(transmission, "syslogv1") == 0)
    {
      int   zslen;
      int   b64len;
      char* zs      = compress_string(jsonStr,&zslen);
      char* b64     = base64_encode(zs, zslen, &b64len);

      asprintf(&cmdline, "XALT_EXECUTABLE_TRACKING=no PATH=%s logger -t XALT_LOGGING_%s \"%s:%s\"\n",XALT_SYSTEM_PATH, syshost, kind, b64);
      system(cmdline);
      free(zs);
      free(b64);
      free(cmdline);
      
    }
  else if (strcasecmp(transmission, "logger") == 0)
    {
      int   sz;
      int   zslen;
      char* zs      = compress_string(jsonStr, &zslen);
      char* b64     = base64_encode(zs, zslen, &sz);
      
      int   blkSz   = (sz < syslog_msg_sz) ? sz : syslog_msg_sz;
      int   nBlks   = (sz -  1)/blkSz + 1;
      int   istrt   = 0;
      int   iend    = blkSz;
      int   i;

      for (i = 0; i < nBlks; i++)
        {
          asprintf(&cmdline, "XALT_EXECUTABLE_TRACKING=no PATH=%s logger -t XALT_LOGGING_%s V:2 kind:%s idx:%d nb:%d syshost:%s key:%s value:%.*s\n",
                   XALT_SYSTEM_PATH, syshost, kind, i, nBlks, syshost, key, iend-istrt, &b64[istrt]);
          system(cmdline);
          free(cmdline);
          istrt = iend;
          iend  = istrt + blkSz;
          if (iend > sz)
            iend = sz;
        }
      free(b64);
    }
  else if (strcasecmp(transmission, "syslog") == 0)
    {
      char* logNm;
      int   sz;
      int   zslen;
      char* zs      = compress_string(jsonStr, &zslen);
      char* b64     = base64_encode(zs, zslen, &sz);
      
      int   blkSz   = (sz < syslog_msg_sz) ? sz : syslog_msg_sz;
      int   nBlks   = (sz -  1)/blkSz + 1;
      int   istrt   = 0;
      int   iend    = blkSz;
      int   i;

      asprintf(&logNm, "XALT_LOGGING_%s",syshost);
      openlog(logNm, 0, LOG_USER);

      for (i = 0; i < nBlks; i++)
        {
          syslog(LOG_INFO, "V:2 kind:%s idx:%d nb:%d syshost:%s key:%s value:%.*s\n",
                   kind, i, nBlks, syshost, key, iend-istrt, &b64[istrt]);
          istrt = iend;
          iend  = istrt + blkSz;
          if (iend > sz)
            iend = sz;
        }
      closelog();
      free(b64);
      free(logNm);
    }
  else if (strcasecmp(transmission, "curl") == 0)
    {
      CURLcode res;
      CURL *hnd;
      struct curl_slist *slist = NULL;
      struct response chunk;
      const char *log_url = NULL;
      const char *status = NULL;

      log_url = getenv("XALT_LOGGING_URL");
      if (log_url == NULL)
        log_url = XALT_LOGGING_URL;

      if (strcasecmp(log_url,"") == 0)  {
        DEBUG0(stderr,"  Logging URL should be provided!\n");
        // Log error to syslog
        asprintf(&cmdline, "XALT_EXECUTABLE_TRACKING=no PATH=%s logger -t XALT_LOGGING_ERROR_%s Logging URL should be provided\n",
                 XALT_SYSTEM_PATH, syshost);
        system(cmdline);
        free(cmdline);
        return;
      }
      
      slist = curl_slist_append(slist, "content-type: application/json");
      chunk.memory = malloc(1);
      chunk.size = 0;

      hnd = curl_easy_init();
      if (hnd) {
        curl_easy_setopt(hnd, CURLOPT_URL, log_url);
        curl_easy_setopt(hnd, CURLOPT_POST, 1);
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonStr);
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, strlen(jsonStr));
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(hnd, CURLOPT_HEADER, 1);
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, _write_callback);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *) &chunk);

        res = curl_easy_perform(hnd);

        if(res != CURLE_OK) {
          // Log error to syslog
	  DEBUG1(stderr,"  curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
          asprintf(&cmdline, "XALT_EXECUTABLE_TRACKING=no PATH=%s logger -t XALT_LOGGING_ERROR_%s curl_easy_perform() failed: %s\n",
                   XALT_SYSTEM_PATH, syshost, curl_easy_strerror(res));
          system(cmdline);
          free(cmdline);
        }
        else {
          strtok(chunk.memory, " ");
          status = strtok(NULL, " ");
          if ((strcmp(status, "100") == 0)) {
            (void)  strtok(NULL, " ");
            status = strtok(NULL, " ");
          }
          if ((strcmp(status, "200") != 0) && (strcmp(status, "201") != 0)) {
	    DEBUG2(stderr,"   HTTP status code %s received from %s\n",status, log_url);
            asprintf(&cmdline, "XALT_EXECUTABLE_TRACKING=no PATH=%s logger -t XALT_LOGGING_ERROR_%s HTTP status code %s received from %s\n",
                     XALT_SYSTEM_PATH, syshost, status, log_url);
            system(cmdline);
            free(cmdline);
          }
        }

        curl_easy_cleanup(hnd);
      }

      free(chunk.memory);
      curl_slist_free_all(slist);
    }
}
