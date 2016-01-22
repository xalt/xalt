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
static int    errfd      = -1;
static double start_time = 0.0;
static double end_time   = 0.0;
static long   my_rank    = 0L;
static long   my_size    = 1L;
static char   path[PATH_MAX];
static char   syshost[SZ];
static char * syshost_option;
static int    reject_flag = 0;

#define HERE printf("%s:%d\n",__FILE__,__LINE__)

int reject(const char *path, const char * hostname)
{
  int     i;
  regex_t regex;
  int     iret;
  int     rejected_host = (hostnameSz != 0);
  char    msgbuf[100];

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

      iret = regexec(&regex, path, 0, NULL, 0);
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
  int    status;
  char * p_dbg;
  char * cmdline;
  char * value;
  const char *  rankA[] = {"PMI_RANK", "OMPI_COMM_WORLD_RANK", "MV2_COMM_WORLD_RANK", NULL}; 
  const char *  sizeA[] = {"PMI_SIZE", "OMPI_COMM_WORLD_SIZE", "MV2_COMM_WORLD_SIZE", NULL}; 
  const char ** p;
  struct timeval tv;
  struct utsname u;

  uuid_t uuid;

  /* Stop tracking if XALT is turned off */
  
  value = getenv("XALT_EXECUTABLE_TRACKING");
  fprintf(stderr,"Test for XALT_EXECUTABLE_TRACKING: \"%s\"\n", value || "(NULL)");
  if (! value)
    return;

  value = getenv("__XALT_INITIAL_STATE__");
  fprintf(stderr,"Test for __XALT_INITIAL_STATE__: \"%s\"\n",value || "(NULL)");
  /* Stop tracking if any myinit routine has been called */
  if (value)
    return;
  setenv("__XALT_INITIAL_STATE__",STR(STATE),1);


  /* Stop tracking if my mpi rank is not zero */
  my_rank = compute_value(rankA);
  fprintf(stderr,"Test for rank == 0, rank: %d\n",my_rank);
  if (my_rank > 0L)
    return;

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

  fprintf(stderr,"Test for path and hostname, hostname: %s, path: %s\n", u.nodename, path);
  reject_flag = reject(path, u.nodename);
  if (reject_flag)
    return;

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

  uuid_generate(uuid);
  uuid_unparse_lower(uuid,uuid_str);
  gettimeofday(&tv,NULL);
  start_time = tv.tv_sec + 1.e-6*tv.tv_usec;

  /* Stop tracking if path is rejected. */
  
  asprintf(&cmdline, "LD_PRELOAD= LD_LIBRARY_PATH=%s PATH= %s -E %s %s --start \"%.3f\" --end 0 -- '[{\"exec_prog\": \"%s\", \"ntasks\": %ld, \"uuid\": \"%s\"}]'",
	   "@sys_ld_lib_path@", "@python@","@PREFIX@/libexec/xalt_run_submission.py", syshost_option, start_time, path, my_size, uuid_str);

  
  p_dbg = getenv("XALT_TRACING");

  if (p_dbg && strcmp(p_dbg,"yes") == 0)
    fprintf(stderr, "xalt_initialize.c:\nStart Tracking: %s\n",cmdline);
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

  /* Stop tracking if my mpi rank is not zero or the path was rejected. */
  if (reject_flag || my_rank > 0L || start_time < 0.01)
    return;

  /* Stop tracking if XALT is turned off */
  if (! getenv("XALT_EXECUTABLE_TRACKING"))
    return;

  /* Stop tracking this initial state does not match STATE that was defined when this routine  was built. */
  v = getenv("__XALT_INITIAL_STATE__");
  if (!v || strcmp(v,STR(STATE)) != 0)
    return;

  my_stderr = fdopen(errfd,"w");
  gettimeofday(&tv,NULL);
  end_time = tv.tv_sec + 1.e-6*tv.tv_usec;

  asprintf(&cmdline, "LD_PRELOAD= LD_LIBRARY_PATH=%s PATH= %s -E %s %s --start \"%.3f\" --end \"%.3f\" -- '[{\"exec_prog\": \"%s\", \"ntasks\": %ld, \"uuid\": \"%s\"}]'",
	   "@sys_ld_lib_path@", "@python@","@PREFIX@/libexec/xalt_run_submission.py", syshost_option, start_time, end_time, path, my_size, uuid_str);

  
  p_dbg = getenv("XALT_TRACING");
  if (p_dbg && strcmp(p_dbg,"yes") == 0)
    fprintf(my_stderr,"\nEnd Tracking: %s\n",cmdline);
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

