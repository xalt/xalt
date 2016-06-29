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
#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include <regex.h>
#include "xalt_regex.h"
#include <errno.h>
#include <sys/utsname.h>
#include <unistd.h>
#ifdef __MACH__
#  include <libproc.h>
#endif
#include "xalt_function_names.h"
#include "xalt_quotestring.h"
#include "xalt_config.h"
#include "xalt_fgets_alloc.h"

const char* xalt_syshost();
int reject(const char *path, const char * hostname);
long compute_value(const char **envA);
void abspath(char * path, int sz);
void myinit(int argc, char **argv);
void myfini();

#define STR(x)  STR2(x)
#define STR2(x) #x
static pid_t  ppid         = 0;
static char   uuid_str[37];
static int    errfd	   = -1;
static double start_time   = 0.0;
static double end_time	   = 0.0;
static long   my_rank	   = 0L;
static long   my_size	   = 1L;
static int    xalt_tracing = 0;
static int    reject_flag  = 0;
static int    background   = 1;
static char   path[PATH_MAX];
static char * usr_cmdline;
static const char * syshost;
#define HERE fprintf(stderr, "%s:%d\n",__FILE__,__LINE__)


#define DEBUG0(fp,s)          if (xalt_tracing) fprintf(fp,s)
#define DEBUG1(fp,s,x1)       if (xalt_tracing) fprintf(fp,s,(x1))
#define DEBUG2(fp,s,x1,x2)    if (xalt_tracing) fprintf(fp,s,(x1),(x2))
#define DEBUG3(fp,s,x1,x2,x3) if (xalt_tracing) fprintf(fp,s,(x1),(x2),(x3))


#ifdef HAVE_LIBUUID
#  include <uuid/uuid.h>
   void build_uuid_str()
   {
     uuid_t uuid;
     uuid_generate(uuid);
     uuid_unparse_lower(uuid,uuid_str);
   }
#else
   void build_uuid_str()
   {
     const char* uuid_proc_fn = "/proc/sys/kernel/random/uuid";
     char*       buf          = NULL;
     size_t      sz           = 0;
     FILE*       fp           = fopen(uuid_proc_fn,"r");

     if (!fp)
       {
         fprintf(stderr,"Unable to open: %s\n",uuid_proc_fn);
         exit(1);
       }

     xalt_fgets_alloc(fp, &buf, &sz);
     memcpy(uuid_str,buf, 36);
     uuid_str[36] = '\0';
     fclose(fp);
   }
#endif


void myinit(int argc, char **argv)
{
  int    status, i;
  char * p;
  char * p_dbg;
  char * cmdline;
  char * v;
  char * ld_preload_strp = NULL;
  const char *  rankA[] = {"PMI_RANK", "OMPI_COMM_WORLD_RANK", "MV2_COMM_WORLD_RANK", NULL};
  const char *  sizeA[] = {"PMI_SIZE", "OMPI_COMM_WORLD_SIZE", "MV2_COMM_WORLD_SIZE", NULL};

  struct timeval tv;
  struct utsname u;

  /* Stop tracking if XALT is turned off */
  p_dbg = getenv("XALT_TRACING");
  if (p_dbg && strcmp(p_dbg,"yes") == 0)
    {
      background   = 0;
      xalt_tracing = 1;
      errfd = dup(STDERR_FILENO);
    }

  v = getenv("XALT_EXECUTABLE_TRACKING");
  if (xalt_tracing)
    {
      abspath(path,sizeof(path));
      DEBUG3(stderr,"myinit(%s,%s):\n  Test for XALT_EXECUTABLE_TRACKING: \"%s\"\n", STR(STATE),path,(v != NULL) ? v : "(NULL)");
    }

  if (!v || strcmp(v,"yes") != 0)
    {
      DEBUG0(stderr,"  XALT_EXECUTABLE_TRACKING is off -> exiting\n\n");
      return;
    }

  v = getenv("__XALT_INITIAL_STATE__");
  DEBUG2(stderr,"  Test for __XALT_INITIAL_STATE__: \"%s\", STATE: \"%s\"\n", (v != NULL) ? v : "(NULL)", STR(STATE));
  /* Stop tracking if any myinit routine has been called */
  if (v && (strcmp(v,STR(STATE)) != 0))
    {
      DEBUG2(stderr," __XALT_INITIAL_STATE__ has a value: \"%s\" -> and it is different from STATE: \"%s\" exiting\n\n",v,STR(STATE));
      return;
    }

  /* Stop tracking if my mpi rank is not zero */
  my_rank = compute_value(rankA);
  DEBUG1(stderr,"  Test for rank == 0, rank: %ld\n",my_rank);
  if (my_rank > 0L)
    {
      DEBUG0(stderr," MPI Rank is not zero -> exiting\n\n");
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
  DEBUG3(stderr,"  Test for path and hostname, hostname: %s, path: %s, reject: %d\n", u.nodename, path, reject_flag);
  if (reject_flag)
    {
      DEBUG0(stderr,"  reject_flag is true -> exiting\n\n");
      return;
    }

  setenv("__XALT_INITIAL_STATE__",STR(STATE),1);
  errfd = dup(STDERR_FILENO);

  syshost = xalt_syshost();

  /* Build a json version of the user's command line. */

  /* Calculate size of buffer*/
  int sz = 0;
  for (i = 0; i < argc; ++i)
    sz += strlen(argv[i]);

  /* this size formula uses 3 facts:
   *   1) if every character was a utf-16 character that is four bytes converts to 12 (sz*3)
   *   2) Each entry has two quotes and a comma (argc*3)
   *   3) There are two square brackets and a null byte (+3)
   */

  sz = sz*3 + argc*3 + 3;

  usr_cmdline = (char *) malloc(sz);
  p	      = &usr_cmdline[0];
  *p++        = '[';
  for (i = 0; i < argc; ++i)
    {
      *p++ = '"';
      const char* qs  = xalt_quotestring(argv[i]);
      int         len = strlen(qs);
      memcpy(p,qs,len);
      p += len;
      *p++= '"';
      *p++= ',';
    }
  *--p = ']';
  *++p = '\0';
  if (p > &usr_cmdline[sz])
    {
      fprintf(stderr,"XALT: Failure in building user command line json string!\n");
      reject_flag = 1;
      return;
    }

  v = getenv("XALT_DISABLE_BACKGROUNDING");
  if (v && strcmp(v,"yes") == 0)
    background = 0;

  build_uuid_str();
  gettimeofday(&tv,NULL);
  start_time = tv.tv_sec + 1.e-6*tv.tv_usec;

  /*************************************************************
   * Save away original PATH and LD_LIBRARY_PATH so that XALT
   * will report what the user had and not what we
   * ran xalt_run_submission with!
   ************************************************************/

  const char * envA[] = {       "PATH",         "LD_LIBRARY_PATH"  };
  const char * envB[] = {"__XALT_PATH_", "__XALT_LD_LIBRARY_PATH_" };
  size_t       envSz  = sizeof(envA)/sizeof(envA[0]);

  size_t ja;
  for (ja = 0; ja < envSz; ++ja)
    {
      char* v = getenv(envA[ja]);
      if (v)
        setenv(envB[ja], v, 1);
    }

  /**********************************************************
   * Save LD_PRELOAD and clear it before running
   * xalt_run_submission.
   *********************************************************/

  p = getenv("LD_PRELOAD");
  if (p)
    ld_preload_strp = strdup(p);

  unsetenv("LD_PRELOAD");

  ppid = getppid();
  asprintf(&cmdline, "LD_LIBRARY_PATH=%s PATH=/usr/bin:/bin %s --ppid %d --syshost \"%s\" --start \"%.3f\" --end 0 --exec \"%s\" --ntasks %ld"
	   " --uuid \"%s\" '%s' %s", SYS_LD_LIB_PATH, PREFIX "/libexec/xalt_run_submission", ppid, syshost,
	   start_time, path, my_size, uuid_str, usr_cmdline, (background ? "&":" "));

  DEBUG1(stderr, "  Start Tracking: %s\nEnd myinit()\n\n",cmdline);
  system(cmdline);
  free(cmdline);

  /**********************************************************
   * Restore LD_PRELOAD after running xalt_run_submission.
   * This way the application and child apps will have
   * LD_PRELOAD set. (I'm looking at you mpiexec.hydra!)
   *********************************************************/

  if (ld_preload_strp)
    {
      setenv("LD_PRELOAD", ld_preload_strp, 1);
      free(ld_preload_strp);
    }
}
void myfini()
{
  FILE * fp;
  FILE * my_stderr;
  char * v;
  char * p_dbg;
  char * cmdline;
  struct timeval tv;

  if (xalt_tracing)
    my_stderr = fdopen(errfd,"w");

  DEBUG3(my_stderr,"\nmyfini():\n  reject_flag: %d, my_rank: %ld, start_time: %f\n", reject_flag, my_rank, start_time);

  /* Stop tracking if my mpi rank is not zero or the path was rejected. */
  if (reject_flag || my_rank > 0L || start_time < 0.01)
    {
      DEBUG0(my_stderr,"    -> exiting\n\n");
      return;
    }


  /* Stop tracking if XALT is turned off */
  v = getenv("XALT_EXECUTABLE_TRACKING");
  DEBUG1(my_stderr,"  Test for XALT_EXECUTABLE_TRACKING: \"%s\"\n", (v != NULL) ? v : "(NULL)");
  if (! v)
    {
      DEBUG0(my_stderr,"   XALT_EXECUTABLE_TRACKING is turned off -> exiting \n\n");
      return;
    }

  /* Stop tracking this initial state does not match STATE that was defined when this routine  was built. */
  v = getenv("__XALT_INITIAL_STATE__");
  DEBUG1(my_stderr,"  Test for __XALT_INITIAL_STATE__: \"%s\"\n", (v != NULL) ? v : "(NULL)");
  DEBUG1(my_stderr,"  STATE: \"%s\"\n", STR(STATE));
  if (!v || strcmp(v,STR(STATE)) != 0)
    {
      DEBUG0(my_stderr,    "STATE and __XALT_INITIAL_STATE__ do not match -> exiting\n\n");
      return;
    }

  gettimeofday(&tv,NULL);
  end_time = tv.tv_sec + 1.e-6*tv.tv_usec;

  unsetenv("LD_PRELOAD");

  /* Do not background this because it might get killed by the epilog cleanup tool! */

  asprintf(&cmdline, "LD_LIBRARY_PATH=%s PATH=/usr/bin:/bin %s --ppid %d --syshost \"%s\" --start \"%.3f\" --end \"%.3f\" --exec \"%s\""
           " --ntasks %ld --uuid \"%s\" '%s'", SYS_LD_LIB_PATH, PREFIX "/libexec/xalt_run_submission", ppid, syshost,
	   start_time, end_time, path, my_size, uuid_str, usr_cmdline);

  DEBUG1(my_stderr,"\nEnd Tracking: %s\n",cmdline);

  system(cmdline);
  free(cmdline);
  free(usr_cmdline);
}

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

  // explain why reject happenned!!!


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
    {
      DEBUG1(stderr,"    hostname: \"%s\" is rejected\n",hostname);
      return 1;
    }

  DEBUG1(stderr,"    hostname: \"%s\" is accepted\n",hostname);
  for (i = 0; i < acceptPathSz; i++)
    {
      iret = regcomp(&regex, acceptPathA[i], 0);
      if (iret)
	{
	  fprintf(stderr,"Could not compile regex: \"%s\n", acceptPathA[i]);
	  exit(1);
	}

      iret = regexec(&regex, path, 0, NULL, 0);
      if (iret == 0)
	{
	  DEBUG1(stderr,"    path: \"%s\" is accepted because of the accept list\n",path);
	  return 0;
	}
      else if (iret != REG_NOMATCH)
	{
	  regerror(iret, &regex, msgbuf, sizeof(msgbuf));
	  fprintf(stderr, "AcceptA Regex match failed: %s\n", msgbuf);
	  exit(1);
	}
      regfree(&regex);
    }

  for (i = 0; i < ignorePathSz; i++)
    {
      iret = regcomp(&regex, ignorePathA[i], 0);
      if (iret)
	{
	  fprintf(stderr,"Could not compile regex: \"%s\n", ignorePathA[i]);
	  exit(1);
	}

      iret = regexec(&regex, path, 0, NULL, 0);
      if (iret == 0)
	{
	  DEBUG2(stderr,"    path: \"%s\" is rejected because of the ignore list: %s\n", path, ignorePathA[i]);
	  return 1;
	}
      else if (iret != REG_NOMATCH)
	{
	  regerror(iret, &regex, msgbuf, sizeof(msgbuf));
	  fprintf(stderr, "IgnoreA Regex match failed: %s\n", msgbuf);
	  exit(1);
	}
      regfree(&regex);
    }
  DEBUG1(stderr,"    path: \"%s\" is accepted because it wasn't found in the ignore list\n",path);
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

#ifdef __MACH__
  __attribute__((section("__DATA,__mod_init_func"), used, aligned(sizeof(void*)))) typeof(myinit) *__init = myinit;
  __attribute__((section("__DATA,__mod_term_func"), used, aligned(sizeof(void*)))) typeof(myfini) *__fini = myfini;
#else
  __attribute__((section(".init_array"))) typeof(myinit) *__init = myinit;
  __attribute__((section(".fini_array"))) typeof(myfini) *__fini = myfini;
#endif

