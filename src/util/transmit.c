#define  _GNU_SOURCE
#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <unistd.h>
#include "transmit.h"
#include "base64.h"
#include "xalt_config.h"
#include "xalt_dir.h"
#include "xalt_c_utils.h"
#include "xalt_base_types.h"

const int syslog_msg_sz = SYSLOG_MSG_SZ;

void transmit(const char* transmission, const char* jsonStr, const char* kind, const char* key,
              const char* crcStr, const char* syshost, char* resultDir, const char* resultFn,
              FILE* my_stderr)
{
  char * logNm   = NULL;
  char * p_dbg        = getenv("XALT_TRACING");
  int    xalt_tracing = (p_dbg && (strncmp(p_dbg,"yes",3)  == 0 ||
				   strncmp(p_dbg,"run",3)  == 0 ));

  if (strcasecmp(transmission,"directdb") == 0)
    {
      DEBUG0(my_stderr,"  Direct to DB transmission is NOT supported!\n");
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
	  DEBUG0(my_stderr,"    resultFn is NULL, $HOME or $USER might be undefined -> No XALT output\n");
	  return;
	}

      int err = mkpath(resultDir, 0700);
      if (err)
	{
	  if (xalt_tracing)
	    {
	      perror("Error: ");
	      fprintf(my_stderr,"    unable to mkpath(%s) -> No XALT output\n", resultDir);
	    }
	  return;
	}

      char* tmpFn = NULL;
      asprintf(&tmpFn, "%s.%s.new",resultDir, resultFn);
      char* fn = NULL;
      asprintf(&fn, "%s%s",resultDir, resultFn);

      FILE* fp = fopen(tmpFn,"w");
      if (fp == NULL && xalt_tracing)
	fprintf(my_stderr,"    Unable to open: %s -> No XALT output\n", fn);
      else
        {
          fprintf(fp, "%s\n", jsonStr);
          fclose(fp);
          rename(tmpFn, fn);
          DEBUG2(my_stderr,"    Wrote json %s file : %s\n",kind, fn);
        }
      my_free(tmpFn,strlen(tmpFn));
      my_free(fn,   strlen(fn));
    }
  else if (strcasecmp(transmission, "syslog") == 0)
    {
      int   sz      = strlen(jsonStr);
      int   blkSz   = (sz < syslog_msg_sz) ? sz : syslog_msg_sz;
      int   nBlks   = (sz -  1)/blkSz + 1;
      int   istrt   = 0;
      int   iend    = blkSz;
      int   i;

      asprintf(&logNm, "XALT_LOGGING_%s",syshost);
      openlog(logNm, 0, LOG_USER);

      for (i = 0; i < nBlks; i++)
        {
          syslog(LOG_INFO, "V:4 kind:%s nb:%d syshost:%s key:%s crc:%s idx:%d value:%.*s",
                                kind,   nBlks,syshost,   key,   crcStr,i,     iend-istrt, &jsonStr[istrt]);
          istrt = iend;
          iend  = istrt + blkSz;
          if (iend > sz)
            iend = sz;
        }
      closelog();
      my_free(logNm, strlen(logNm));
    }
  else if (strcasecmp(transmission, "curl") == 0)
    {
      int pid, status, ret = 0;
      char *myargs [] 	= { NULL, NULL, NULL, NULL};
      char *myenv  [] 	= { NULL };

      // Prepend $LIB64 to $LD_LIBRARY_PATH
      int  i;
      char *ld_lib_path = getenv("LD_LIBRARY_PATH");
      char *lib64_dir   = xalt_dir("lib64");
      int  len_lpath    = strlen(ld_lib_path);
      int  len_l64dir   = strlen(lib64_dir);
      int  len          = len_l64dir + len_lpath + 2;
      char *value       = (char *) malloc(len*sizeof(char));

      i = 0;
      memcpy(&value[i], lib64_dir,   len_l64dir); i += len_l64dir;
      value[i] = ':';                             i += 1;
      memcpy(&value[i], ld_lib_path, len_lpath);  i += len_lpath;
      value[i] = '\0';                            i += 1;
      setenv("LD_LIBRARY_PATH", value, 1);
      my_free(value,len);

      // Define arguments to xalt_curl_transmit.c
      char* prgm = xalt_dir("libexec/xalt_curl_transmit"); 
      myargs[0]  = prgm;
      myargs[1]  = (char *) syshost;
      myargs[2]  = (char *) jsonStr;

      // Call xalt_curl_transmit as a child process and wait for it to complete.
      pid = fork();
      if (pid == 0)
	{
	  // Child process
	  execve(prgm, myargs, myenv);
	}

      // Parent: Wait for child to complete
      if ((ret = waitpid (pid, &status, 0)) == -1)
	{
	  DEBUG0(my_stderr, "  waitpid() returned -1: error with xalt_curl_transmit\n");
	  asprintf(&logNm, "XALT_LOGGING_ERROR_%s",syshost);
	  openlog(logNm, LOG_PID, LOG_USER);
	  syslog(LOG_INFO, "waitpid() returned -1: error with xalt_curl_transmit");
	  closelog();
	  my_free(logNm,strlen(logNm));
	  return;
	}

      // Free memory
      my_free(prgm, strlen(prgm));
    }
  else if (strcasecmp(transmission, "syslogv1") == 0)
    {
      asprintf(&logNm, "XALT_LOGGING_%s",syshost);
      openlog(logNm, 0, LOG_USER);

      syslog(LOG_INFO, "%s:%s", kind, &jsonStr[0]);
      closelog();
      my_free(logNm, strlen(logNm));
    }
  else if (strcasecmp(transmission, "logger") == 0)
    {
      int         i, pid, status, ret = 0;
      char        *myargs [] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
      char        *myenv  [] = { NULL };
      const char  *prgm      = "logger";
      char        *tagStr;
      char        *kindStr;
      char        *nbStr;
      char        *keyStr;
      char        *crcS;
      char        *syshostStr;
      char        idxStr[20];

      unsetenv("LD_PRELOAD");
      setenv("XALT_EXECUTABLE_TRACKING","no",1);
      setenv("PATH",XALT_SYSTEM_PATH,1);

      int   sz      = strlen(jsonStr);
      while(isspace(jsonStr[sz-1]))
	--sz;

      int   blkSz   = (sz < syslog_msg_sz) ? sz : syslog_msg_sz;
      int   nBlks   = (sz -  1)/blkSz + 1;

      
      fprintf(stderr,"crcStr:%s\n", crcStr);
      asprintf(&tagStr,     "XALT_LOGGING_%s", syshost);
      asprintf(&kindStr,    "kind:%s",         kind);
      asprintf(&nbStr,      "nb:%d",           nBlks);
      asprintf(&syshostStr, "syshost:%s",      syshost);
      asprintf(&keyStr,     "key:%s",          key);
      asprintf(&crcS,       "crcStr:%s",       crcStr);

      int istrt	     = 0;
      int iend	     = blkSz;
      int  nvalueStr = syslog_msg_sz + 7;
      char *valueStr = (char *) malloc(nvalueStr);

      myargs[0]	     = (char *) prgm;
      myargs[1]	     = "-t";
      myargs[2]	     = tagStr;
      myargs[3]	     = "V:4";
      myargs[4]	     = kindStr;
      myargs[5]	     = nbStr;
      myargs[6]	     = syshostStr;
      myargs[7]	     = keyStr;
      myargs[8]	     = crcS;
      myargs[9]	     = idxStr;
      myargs[10]     = valueStr;
      
      for (i = 0; i < nBlks; i++)
        {
	  sprintf(&idxStr[0],   "idx:%d",     i);
	  sprintf(&valueStr[0], "value:%.*s", iend-istrt, &jsonStr[istrt]);

	  pid = fork();
	  if (pid == 0)
	    {
	      // Child process
	      execvpe(prgm, myargs, myenv);
	    }

	  // Parent: Wait for child to complete
	  if ( (ret = waitpid(pid, &status, 0)) == -1)
	    {
	      DEBUG1(my_stderr, "  waitpid() returned %d: error with logger\n", ret);
	      return;
	    }
	      
          istrt = iend;
          iend  = istrt + blkSz;
          if (iend > sz)
            iend = sz;
        }
      my_free(tagStr,  	  strlen(tagStr));
      my_free(kindStr, 	  strlen(kindStr));
      my_free(nbStr,   	  strlen(nbStr));
      my_free(syshostStr, strlen(syshostStr));
      my_free(keyStr,     strlen(keyStr));
      my_free(valueStr,   nvalueStr);
    }
}
