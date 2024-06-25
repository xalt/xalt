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
#include "xalt_debug_macros.h"

#define XALT_LOGGING_LBL  "XALT_LOGGING"
#define KIND_LBL          "kind"
#define NB_LBL            "nb"
#define SYSHOST_LBL       "syshost"
#define KEY_LBL           "key"
#define CRC_LBL           "crcStr"
#define IDX_LBL           "idx"
#define VALUE_LBL         "value"
#define V_LBL             "V"
#define INTERFACE_Version  4
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
      DEBUG(my_stderr,"  Direct to DB transmission is NOT supported!\n");
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
          DEBUG(my_stderr,"    resultFn is NULL, $HOME or $USER might be undefined -> No XALT output\n");
          return;
        }

      int err = mkpath(resultDir, 0700);
      if (err)
        {
          if (xalt_tracing)
            {
              perror("Error: ");
              DEBUG(my_stderr,"    unable to mkpath(%s) -> No XALT output\n", resultDir);
            }
          return;
        }

      char* tmpFn = NULL;
      asprintf(&tmpFn, "%s.%s.new",resultDir, resultFn);
      char* fn = NULL;
      asprintf(&fn, "%s%s",resultDir, resultFn);

      FILE* fp = fopen(tmpFn,"w");
      if (fp == NULL)
        {
          DEBUG(my_stderr,"    Unable to open: %s -> No XALT output\n", fn);
        }
      else
        {
          fprintf(fp, "%s\n", jsonStr);
          fclose(fp);
          rename(tmpFn, fn);
          DEBUG(my_stderr,"    Using %s method to store json file\n",xalt_file_transmission_method());
          DEBUG(my_stderr,"    Wrote json %s file : %s\n",kind, fn);
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

      asprintf(&logNm, "%s_%s", XALT_LOGGING_LBL, syshost);
      openlog(logNm, 0, LOG_USER);

      for (i = 0; i < nBlks; i++)
        {
          /*                   V  kind  host  key   crc   nb      idx     value */                                     
          syslog(LOG_INFO, "%s:%d %s:%s %s:%s %s:%s %s:%s %s:%02d %s:%02d %s:%.*s",
                 V_LBL,       INTERFACE_Version,
                 KIND_LBL,    kind,
                 SYSHOST_LBL, syshost,
                 KEY_LBL,     key,
                 CRC_LBL,     crcStr,
                 NB_LBL,      nBlks,
                 IDX_LBL,     i,
                 VALUE_LBL,   iend-istrt, &jsonStr[istrt]);
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
      char *myargs []   = { NULL, NULL, NULL, NULL};
      char *myenv  []   = { NULL };

      // Prepend $LIB64 to $LD_LIBRARY_PATH
      int  i;
      char *ld_lib_path = getenv("LD_LIBRARY_PATH");
      char *lib64_dir   = xalt_dir("lib64");
      int  len_lpath    = strlen(ld_lib_path);
      int  len_l64dir   = strlen(lib64_dir);
      int  len          = len_l64dir + len_lpath + 2;
      char *value       = (char *) XMALLOC(len*sizeof(char));

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
          DEBUG(my_stderr, "  waitpid() returned -1: error with xalt_curl_transmit\n");
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
      char        *interfaceStr;
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

      asprintf(&tagStr,       "%s_%s",   XALT_LOGGING_LBL, syshost);
      asprintf(&interfaceStr, "%s:%d",   V_LBL,            INTERFACE_Version);
      asprintf(&kindStr,      "%s:%s",   KIND_LBL,         kind);
      asprintf(&syshostStr,   "%s:%s",   SYSHOST_LBL,      syshost);
      asprintf(&keyStr,       "%s:%s",   KEY_LBL,          key);
      asprintf(&crcS,         "%s:%s",   CRC_LBL,          crcStr);
      asprintf(&nbStr,        "%s:%02d", NB_LBL,           nBlks);

      int istrt      = 0;
      int iend       = blkSz;
      int  nvalueStr = syslog_msg_sz + 7;
      char *valueStr = (char *) XMALLOC(nvalueStr);

      myargs[ 0] = (char *) prgm;
      myargs[ 1] = "-t";
      myargs[ 2] = tagStr;
      myargs[ 3] = interfaceStr;
      myargs[ 4] = kindStr;
      myargs[ 5] = syshostStr;
      myargs[ 6] = keyStr;
      myargs[ 7] = crcS;
      myargs[ 8] = nbStr;
      myargs[ 9] = idxStr;
      myargs[10] = valueStr;

      for (i = 0; i < nBlks; i++)
        {
          sprintf(&idxStr[0],   "%s:%02d", IDX_LBL,   i);
          sprintf(&valueStr[0], "%s:%.*s", VALUE_LBL, iend-istrt, &jsonStr[istrt]);

          pid = fork();
          if (pid == 0)
            {
              // Child process
              execvpe(prgm, myargs, myenv);
            }

          // Parent: Wait for child to complete
          if ( (ret = waitpid(pid, &status, 0)) == -1)
            {
              DEBUG(my_stderr, "  waitpid() returned %d: error with logger\n", ret);
              return;
            }

          istrt = iend;
          iend  = istrt + blkSz;
          if (iend > sz)
            iend = sz;
        }
      my_free(tagStr,       strlen(tagStr));
      my_free(interfaceStr, strlen(interfaceStr));
      my_free(kindStr,      strlen(kindStr));
      my_free(nbStr,        strlen(nbStr));
      my_free(syshostStr,   strlen(syshostStr));
      my_free(keyStr,       strlen(keyStr));
      my_free(valueStr,     nvalueStr);
      my_free(crcS,         strlen(crcS));
    }
}
