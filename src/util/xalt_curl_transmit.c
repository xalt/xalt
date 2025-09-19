#define  _GNU_SOURCE
#ifdef HAVE_CURL_CURL_H
#   include <curl/curl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include "xalt_config.h"
#include "xalt_c_utils.h"
#include "xalt_debug_macros.h"

const int syslog_msg_sz = SYSLOG_MSG_SZ;

struct response {
  char *memory;
  size_t size;
};

#ifndef HAVE_CURL_CURL_H
int main(int argc, char* argv[])
{
  return 0;
}
#else    
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

int main(int argc, char* argv[])
{
  char *logNm        = NULL;
  char *syshost      = argv[1];
  char *jsonStr      = argv[2];
  char *p_dbg        = xalt_getenv("XALT_TRACING");
  int   xalt_tracing = (p_dbg && (strncmp(p_dbg,"yes",3)  == 0 ||
                                  strncmp(p_dbg,"run",3)  == 0 ));

  CURLcode res;
  CURL *hnd;
  struct curl_slist *slist = NULL;
  struct response chunk;
  const char *log_url = NULL;
  const char *status = NULL;
  
  log_url = xalt_getenv("XALT_LOGGING_URL");
  if (log_url == NULL)
    log_url = XALT_LOGGING_URL;
  
  if (strcasecmp(log_url,"") == 0)
    {
      DEBUG(stderr,"  Logging URL should be provided!\n");
      asprintf(&logNm, "XALT_LOGGING_ERROR_%s",syshost);
      openlog(logNm, LOG_PID, LOG_USER);
      syslog(LOG_INFO, "Logging URL should be provided");
      closelog();
      free(logNm);
      return 1;
    }
      
  slist = curl_slist_append(slist, "content-type: application/json");
  chunk.memory = XMALLOC(1);
  chunk.size = 0;
  
  hnd = curl_easy_init();
  if (hnd)
    {
      curl_easy_setopt(hnd, CURLOPT_URL, log_url);
      curl_easy_setopt(hnd, CURLOPT_POST, 1);
      curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonStr);
      curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE, strlen(jsonStr));
      curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist);
      curl_easy_setopt(hnd, CURLOPT_HEADER, 1);
      curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, _write_callback);
      curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *) &chunk);

      res = curl_easy_perform(hnd);

      if(res != CURLE_OK)
	{
	  // Log error to syslog
	  DEBUG(stderr,"  curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
          asprintf(&logNm, "XALT_LOGGING_ERROR_%s",syshost);
          openlog(logNm, LOG_PID, LOG_USER);
          syslog(LOG_INFO, "curl_easy_perform() failed: %s",curl_easy_strerror(res));
          closelog();
          free(logNm);
        }
      else
	{
          strtok(chunk.memory, " ");
          status = strtok(NULL, " ");
          if ((strcmp(status, "100") == 0))
	    {
	      (void)  strtok(NULL, " ");
	      status = strtok(NULL, " ");
	    }
          if ((strcmp(status, "200") != 0) && (strcmp(status, "201") != 0))
	    {
	      DEBUG(stderr,"   HTTP status code %s received from %s\n",status, log_url);
	      asprintf(&logNm, "XALT_LOGGING_ERROR_%s",syshost);
	      openlog(logNm, LOG_PID, LOG_USER);
	      syslog(LOG_INFO, "HTTP status code %s received from %s",status, log_url);
	      closelog();
	      free(logNm);
	    }
        }
      curl_easy_cleanup(hnd);
    }

  free(chunk.memory);
  curl_slist_free_all(slist);
  return 0;
}
#endif
