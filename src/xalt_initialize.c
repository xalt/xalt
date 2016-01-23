/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


#define  _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>
#include <time.h>
#include <limits.h>
#include <regex.h>
#include "xalt_regex.h"
#include <errno.h>
#include <sys/utsname.h>
#ifdef __MACH__
#  include <libproc.h>
#endif



#define STR(x)  STR2(x)
#define STR2(x) #x
#define SZ      256
static char   uuid_str[37];
static int    errfd	   = -1;
static double start_time   = 0.0;
static double end_time	   = 0.0;
static long   my_rank	   = 0L;
static long   my_size	   = 1L;
static int    xalt_tracing = 0;
static int    reject_flag  = 0;
static char   path[PATH_MAX];
static char   syshost[SZ];
static char * syshost_option;
static char   usr_cmdline[65535];
#define HERE printf("%s:%d\n",__FILE__,__LINE__)


#define DEBUG0(fp,s)          if (xalt_tracing) fprintf(fp,s)
#define DEBUG1(fp,s,x1)       if (xalt_tracing) fprintf(fp,s,(x1))
#define DEBUG2(fp,s,x1,x2)    if (xalt_tracing) fprintf(fp,s,(x1),(x2))
#define DEBUG3(fp,s,x1,x2,x3) if (xalt_tracing) fprintf(fp,s,(x1),(x2),(x3))

#ifdef FULL_TRACING
#  define FULL_DEBUG0(fp,s)          DEBUG0(fp,s)
#  define FULL_DEBUG1(fp,s,x1)       DEBUG1(fp,s,(x1))
#  define FULL_DEBUG2(fp,s,x1,x2)    DEBUG2(fp,s,(x1),(x2))
#  define FULL_DEBUG3(fp,s,x1,x2,x3) DEBUG3(fp,s,(x1),(x2),(x3))
#else
#  define FULL_DEBUG0(fp,s)
#  define FULL_DEBUG1(fp,s,x1)    
#  define FULL_DEBUG2(fp,s,x1,x2) 
#  define FULL_DEBUG3(fp,s,x1,x2,x3) 
#endif


int reject(const char *path, const char * hostname)
{
  int     i;
  regex_t regex;
  int     iret;
  int     rejected_host = (hostnameSz != 0);
  char    msgbuf[100];
  char *  p;

  if (path[0] == '\0')
    return 1;
  
  for (i = 0; i < hostnameSz; i++)
    {
      iret = regcomp(&regex, hostnameA[i], 0);
      if (iret)
	{
	  fprintf(stderr,"Could not compile regex: \"%s\n", hostnameA[i]);
	  exit(1);
	}

      iret = regexec(&regex, hostname, 0, NULL, 0);
      if (iret == 0)
	{
	  rejected_host = 0;
	  break;
	}
      else if (iret != REG_NOMATCH)
	{
	  regerror(iret, &regex, msgbuf, sizeof(msgbuf));
	  fprintf(stderr, "HostnameA Regex match failed: %s\n", msgbuf);
	  exit(1);
	}
      regfree(&regex);
    }

  if (rejected_host)
    return 1;

  for (i = 0; i < acceptSz; i++)
    {
      iret = regcomp(&regex, acceptA[i], 0);
      if (iret)
	{
	  fprintf(stderr,"Could not compile regex: \"%s\n", acceptA[i]);
	  exit(1);
	}

      iret = regexec(&regex, path, 0, NULL, 0);
      if (iret == 0)
	return 0;
      else if (iret != REG_NOMATCH)
	{
	  regerror(iret, &regex, msgbuf, sizeof(msgbuf));
	  fprintf(stderr, "AcceptA Regex match failed: %s\n", msgbuf);
	  exit(1);
	}
      regfree(&regex);
    }
  
  for (i = 0; i < ignoreSz; i++)
    {
      iret = regcomp(&regex, ignoreA[i], 0);
      if (iret)
	{
	  fprintf(stderr,"Could not compile regex: \"%s\n", acceptA[i]);
	  exit(1);
	}

      iret = regexec(&regex, path, 0, NULL, 0);
      if (iret == 0)
	return 1;
      else if (iret != REG_NOMATCH)
	{
	  regerror(iret, &regex, msgbuf, sizeof(msgbuf));
	  fprintf(stderr, "IgnoreA Regex match failed: %s\n", msgbuf);
	  exit(1);
	}
      regfree(&regex);
    }
  return 0;
}

long compute_value(const char **envA)
{
  long          value = 0L;
  const char ** p;
  for (p = &envA[0]; *p; ++p)
    {
      char *v = getenv(*p);
      if (v)
        value += strtol(v, (char **) NULL, 10);
    }

  return value;
}

/* Get full absolute path to executable */
/* works for Linux and Mac OS X */
void abspath(char * path, int sz)
{
  path[0] = '\0';
  #ifdef __MACH__
    int iret = proc_pidpath(getpid(), path, sz-1);
    if (iret <= 0)
      {
	fprintf(stderr,"PID %d: proc_pid();\n",getpid());
	fprintf(stderr,"    %s:\n", strerror(errno));
      }
  #else
    readlink("/proc/self/exe",path,sz-1);
  #endif
}


void myinit(int argc, char **argv)
{
  int    status, i;
  char * p;
  char * p_dbg;
  char * cmdline;
  char * v;
  const char *  rankA[] = {"PMI_RANK", "OMPI_COMM_WORLD_RANK", "MV2_COMM_WORLD_RANK", NULL}; 
  const char *  sizeA[] = {"PMI_SIZE", "OMPI_COMM_WORLD_SIZE", "MV2_COMM_WORLD_SIZE", NULL}; 
  
  struct timeval tv;
  struct utsname u;

  uuid_t uuid;

  /* Stop tracking if XALT is turned off */
  
  p_dbg = getenv("XALT_TRACING");
  if (p_dbg && strcmp(p_dbg,"yes") == 0)
    xalt_tracing = 1;
  

  abspath(path,sizeof(path));
  v = getenv("XALT_EXECUTABLE_TRACKING");
  FULL_DEBUG3(stderr,"myinit(%s,%s):\n  Test for XALT_EXECUTABLE_TRACKING: \"%s\"\n", STR(STATE),path,(v != NULL) ? v : "(NULL)");

  if (! v)
    {
      FULL_DEBUG0(stderr,"  XALT_EXECUTABLE_TRACKING is off -> exiting\n");
      return;
    }

  v = getenv("__XALT_INITIAL_STATE__");
  FULL_DEBUG1(stderr,"  Test for __XALT_INITIAL_STATE__: \"%s\"\n", (v != NULL) ? v : "(NULL)");
  /* Stop tracking if any myinit routine has been called */
  if (v)
    {
      FULL_DEBUG0(stderr," __XALT_INITIAL_STATE__ has a value -> exiting\n");
      return;
    }
  setenv("__XALT_INITIAL_STATE__",STR(STATE),1);


  /* Stop tracking if my mpi rank is not zero */
  my_rank = compute_value(rankA);
  FULL_DEBUG1(stderr,"  Test for rank == 0, rank: %ld\n",my_rank);
  if (my_rank > 0L)
    {
      FULL_DEBUG0(stderr," MPI Rank is not zero -> exiting\n");
      return;
    }

  my_size = compute_value(sizeA);
  if (my_size < 1L)
    my_size = 1L;

  errno = 0;
  if (uname(&u) != 0)
    {
      perror("uname");
      exit(EXIT_FAILURE);
    }


  /* Get full absolute path to executable */
  abspath(path,sizeof(path));

  reject_flag = reject(path, u.nodename);
  FULL_DEBUG3(stderr,"  Test for path and hostname, hostname: %s, path: %s, reject: %d\n", u.nodename, path, reject_flag);
  if (reject_flag)
    {
      FULL_DEBUG0(stderr,"  reject_flag is true -> exiting\n");
      return;
    }

  errfd = dup(STDERR_FILENO);

  asprintf(&syshost_option,"%s"," ");
#ifdef HAVE_SYSHOST_CMD
  asprintf(&cmdline,"LD_PRELOAD= LD_LIBRARY_PATH=%s PATH= %s -E %s",
	   "@sys_ld_lib_path@", "@python@",
	   "@PREFIX@/site/xalt_syshost.py");
  FILE* fp = popen(cmdline, "r");
  if (fp)
    {
      int len;
      while(fgets(syshost,SZ, fp) != NULL)
	;
      len = strlen(syshost);
      if(syshost[len-1] == '\n')
	syshost[len-1] = '\0';
      asprintf(&syshost_option,"--syshost %s",syshost);
      fclose(fp);
    }
#endif

  /* Build a json version of the command line.  There is special treatment of " and \\ */
  p = &usr_cmdline[0];
  *p++ = '[';
  for (i = 0; i < argc; i++)
    {
      char* s   = argv[i];
      int   len = strcspn(s,"\"\\");
      *p++ = '"';

      while(1)
	{
	  char c = s[len];
	  memcpy(p,s,len);
	  p += len;
	  s += len+1;
	  if (c == '\0')
	    break;
	  *p++ = '\\';
	  *p++ = c;
	  
	  len = strcspn(s,"\"\\");
	}
      *p++= '"';
      *p++= ',';
    }
  *--p = ']'; p++;
  *p = '\0';
  
  uuid_generate(uuid);
  uuid_unparse_lower(uuid,uuid_str);
  gettimeofday(&tv,NULL);
  start_time = tv.tv_sec + 1.e-6*tv.tv_usec;

  /* Stop tracking if path is rejected. */
  
  asprintf(&cmdline, "LD_PRELOAD= LD_LIBRARY_PATH=%s PATH= %s -E %s %s --start \"%.3f\" --end 0 -- '[{\"exec_prog\": \"%s\", \"ntasks\": %ld, \"uuid\": \"%s\"}]' '%s'",
	   "@sys_ld_lib_path@", "@python@","@PREFIX@/libexec/xalt_run_submission.py", syshost_option, start_time, path, my_size, uuid_str, usr_cmdline);

  
  
  DEBUG1(stderr, "  Start Tracking: %s\nEnd myinit()\n",cmdline);
  system(cmdline);
  free(cmdline);
}

void myfini()
{
  FILE * my_stderr;
  char * v;
  char * p_dbg;
  char * cmdline;
  struct timeval tv;

  if (xalt_tracing)
    my_stderr = fdopen(errfd,"w");

  FULL_DEBUG3(my_stderr,"Starting myfini: reject_flag: %d, my_rank: %ld, start_time: %f",reject_flag, my_rank, start_time);

  /* Stop tracking if my mpi rank is not zero or the path was rejected. */
  if (reject_flag || my_rank > 0L || start_time < 0.01)
    return;


  /* Stop tracking if XALT is turned off */
  v = getenv("XALT_EXECUTABLE_TRACKING");
  FULL_DEBUG1(stderr,"Test for XALT_EXECUTABLE_TRACKING: \"%s\"\n", (v != NULL) ? v : "(NULL)");
  if (! v)
    return;

  /* Stop tracking this initial state does not match STATE that was defined when this routine  was built. */
  v = getenv("__XALT_INITIAL_STATE__");
  FULL_DEBUG1(stderr,"Test for __XALT_INITIAL_STATE__: \"%s\"\n", (v != NULL) ? v : "(NULL)");
  FULL_DEBUG1(stderr,"STATE: \"%s\"\n", STR(STATE));
  if (!v || strcmp(v,STR(STATE)) != 0)
    return;

  gettimeofday(&tv,NULL);
  end_time = tv.tv_sec + 1.e-6*tv.tv_usec;

  asprintf(&cmdline, "LD_PRELOAD= LD_LIBRARY_PATH=%s PATH= %s -E %s %s --start \"%.3f\" --end \"%.3f\" -- '[{\"exec_prog\": \"%s\", \"ntasks\": %ld, \"uuid\": \"%s\"}]' '%s'",
	   "@sys_ld_lib_path@", "@python@","@PREFIX@/libexec/xalt_run_submission.py", syshost_option, start_time, end_time, path, my_size, uuid_str, usr_cmdline);

  DEBUG1(my_stderr,"\nEnd Tracking: %s\n",cmdline);

  system(cmdline);
  free(cmdline);
  free(syshost_option);
}

#ifdef __MACH__
  __attribute__((section("__DATA,__mod_init_func"), used, aligned(sizeof(void*)))) typeof(myinit) *__init = myinit;
  __attribute__((section("__DATA,__mod_term_func"), used, aligned(sizeof(void*)))) typeof(myfini) *__fini = myfini;
#else
  __attribute__((section(".init_array"))) typeof(myinit) *__init = myinit;
  __attribute__((section(".fini_array"))) typeof(myfini) *__fini = myfini;
#endif

