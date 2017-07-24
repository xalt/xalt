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
#include "xalt_quotestring.h"
#include "xalt_config.h"
#include "xalt_fgets_alloc.h"
#include "xalt_obfuscate.h"

#define DATESZ    100

typedef enum { XALT_SUCCESS, XALT_TRACKING_OFF, XALT_WRONG_STATE, XALT_RUN_TWICE,
               XALT_MPI_RANK, XALT_HOSTNAME, XALT_PATH, XALT_BAD_JSON_STR, XALT_MPI_SIZE_ZERO} xalt_status;

static const char * xalt_reasonA[] = {
  "Successful XALT tracking",
  "XALT_EXECUTABLE_TRACKING is off",
  "__XALT_INITIAL_STATE__ is different from STATE",
  "XALT is trying to be twice in the same STATE",
  "XALT only tracks rank 0, in mpi programs",
  "XALT has found the host does not match hostname pattern",
  "XALT has found the executable does not match path pattern",
  "XALT has problem with a JSON string",
  "XALT in tracking MPI only, non-mpi executable found",
};

const  char*           xalt_syshost();
static xalt_status     reject(const char *path, const char * hostname);
static long            compute_value(const char **envA);
static void            abspath(char * path, int sz);
static volatile double epoch();

void myinit(int argc, char **argv);
void myfini();

#define STR(x)   StR1_(x)
#define StR1_(x) #x

static int          countA[2];
static char         uuid_str[37];
static char         exec_path[PATH_MAX];
static char *       usr_cmdline;
static const char * my_syshost;

static xalt_status  reject_flag  = XALT_SUCCESS;
static pid_t        ppid         = 0;
static int          errfd	 = -1;
static double       start_time   = 0.0;
static double       end_time	 = 0.0;
static long         my_rank	 = 0L;
static long         my_size	 = 1L;
static int          xalt_tracing = 0;
static int          background   = 1;
static char *       pathArg      = NULL;
static char *       ldLibPathArg = NULL;

#define HERE fprintf(stderr, "%s:%d\n",__FILE__,__LINE__)

#define DEBUG0(fp,s)             if (xalt_tracing) fprintf((fp),s)
#define DEBUG1(fp,s,x1)          if (xalt_tracing) fprintf((fp),s,(x1))
#define DEBUG2(fp,s,x1,x2)       if (xalt_tracing) fprintf((fp),s,(x1),(x2))
#define DEBUG3(fp,s,x1,x2,x3)    if (xalt_tracing) fprintf((fp),s,(x1),(x2),(x3))
#define DEBUG4(fp,s,x1,x2,x3,x4) if (xalt_tracing) fprintf((fp),s,(x1),(x2),(x3),(x4))

#ifdef HAVE_WORKING_LIBUUID
#  include <uuid/uuid.h>
   void build_uuid_str(char * my_uuid_str)
   {
     uuid_t uuid;
     uuid_generate(uuid);
     uuid_unparse_lower(uuid, my_uuid_str);
   }
#else
   void build_uuid_str(char * my_uuid_str)
   {
     const char* uuid_proc_fn = "/proc/sys/kernel/random/uuid";
     char*       buf          = NULL;
     size_t      sz           = 0;
     FILE*       fp           = fopen(uuid_proc_fn,"r");

     HERE; 
     if (!fp)
       {
         fprintf(stderr,"Unable to open: %s\n",uuid_proc_fn);
         exit(1);
       }

     xalt_fgets_alloc(fp, &buf, &sz);
     memcpy(my_uuid_str,buf, 36);
     my_uuid_str[36] = '\0';
     HERE; fclose(fp);
   }
#endif

void myinit(int argc, char **argv)
{
  int    i;
  char * p;
  char * p_dbg;
  char * cmdline;
  char * v;
  char * ld_preload_strp = NULL;
  char   dateStr[DATESZ];
  const char *  rankA[] = {"PMI_RANK", "OMPI_COMM_WORLD_RANK", "MV2_COMM_WORLD_RANK", NULL};
  const char *  sizeA[] = {"PMI_SIZE", "OMPI_COMM_WORLD_SIZE", "MV2_COMM_WORLD_SIZE", NULL};

  struct utsname u;

  p_dbg = getenv("XALT_TRACING");
  if (p_dbg && (strcmp(p_dbg,"yes") == 0 || strcmp(p_dbg,"run")))
    {
      background   = 0;
      xalt_tracing = 1;
      errfd = dup(STDERR_FILENO);
    }

  v = getenv("__XALT_INITIAL_STATE__");
  if (xalt_tracing)
    {
      if (!v) 
        {
          time_t    now    = (time_t) epoch();
          strftime(dateStr, DATESZ, "%c", localtime(&now));
          errno = 0;
          if (uname(&u) != 0)
            {
              perror("uname");
              exit(EXIT_FAILURE);
            }
          fprintf(stderr, "---------------------------------------------\n"
                          " Date:          %s\n"
                          " XALT Version:  %s\n"
                          " Nodename:      %s\n"
                          " System:        %s\n"
                          " Release:       %s\n"
                          " O.S. Version:  %s\n"
                          " Machine:       %s\n"
		          " Syshost:       %s\n"
                          "---------------------------------------------\n\n",
                  dateStr, XALT_GIT_VERSION, u.nodename, u.sysname, u.release,
                  u.version, u.machine, xalt_syshost());
        }
      abspath(exec_path,sizeof(exec_path));
      fprintf(stderr,"myinit(%s,%s){\n", STR(STATE),exec_path);
    }

  DEBUG2(stderr,"  Test for __XALT_INITIAL_STATE__: \"%s\", STATE: \"%s\"\n", (v != NULL) ? v : "(NULL)", STR(STATE));
  /* Stop tracking if any myinit routine has been called */
  if (v && (strcmp(v,STR(STATE)) != 0))
    {
      DEBUG2(stderr,"    -> __XALT_INITIAL_STATE__ has a value: \"%s\" -> and it is different from STATE: \"%s\" exiting\n}\n\n",v,STR(STATE));
      reject_flag = XALT_WRONG_STATE;
      return;
    }

  /* Stop tracking if XALT is turned off */
  v = getenv("XALT_EXECUTABLE_TRACKING");
  DEBUG1(stderr,"  Test for XALT_EXECUTABLE_TRACKING: %s\n",(v != NULL) ? v : "(NULL)");
  if (!v || strcmp(v,"yes") != 0)
    {
      DEBUG0(stderr,"    -> XALT_EXECUTABLE_TRACKING is off -> exiting\n}\n\n");
      reject_flag = XALT_TRACKING_OFF;
      return;
    }

  if (countA[IDX] > 0)
    {
      DEBUG2(stderr,"    -> countA[%d]: %d which is greater than 0 -> exiting\n}\n\n",IDX,countA[IDX]);
      reject_flag = XALT_RUN_TWICE;
      return;
    }
  countA[IDX]++;

  /* Stop tracking if my mpi rank is not zero */
  my_rank = compute_value(rankA);
  DEBUG1(stderr,"  Test for rank == 0, rank: %ld\n",my_rank);
  if (my_rank > 0L)
    {
      DEBUG0(stderr,"    -> MPI Rank is not zero -> exiting\n}\n\n");
      reject_flag = XALT_MPI_RANK;
      return;
    }

  my_size = compute_value(sizeA);
  v = getenv("XALT_TRACKING_MPI_ONLY");
  if (!v)
    v = XALT_TRACKING_MPI_ONLY;
      
  if (my_size == 0L && (strcmp(v,"yes") == 0))
    {
      DEBUG0(stderr,"    -> MPI Tracking turned on and this job is not an MPI job -> exiting\n}\n\n");
      reject_flag = XALT_MPI_SIZE_ZERO;
      return;
    }


  if (my_size < 1L)
    my_size = 1L;

  errno = 0;
  if (uname(&u) != 0)
    {
      perror("uname");
      exit(EXIT_FAILURE);
    }

  /* Get full absolute path to executable */
  abspath(exec_path,sizeof(exec_path));
  reject_flag = reject(exec_path, u.nodename);
  DEBUG3(stderr,"  Test for path and hostname, hostname: %s, path: %s, reject: %d\n", u.nodename, exec_path, reject_flag);
  if (reject_flag != XALT_SUCCESS)
    {
      DEBUG0(stderr,"    -> reject_flag is true -> exiting\n}\n\n");
      return;
    }

  setenv("__XALT_INITIAL_STATE__",STR(STATE),1);
  errfd = dup(STDERR_FILENO);

  my_syshost = xalt_syshost();

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
      reject_flag = XALT_BAD_JSON_STR;
      return;
    }

  v = getenv("XALT_DISABLE_BACKGROUNDING");
  if (v && strcmp(v,"yes") == 0)
    background = 0;

  
  build_uuid_str(uuid_str);
  start_time = epoch();

  /**********************************************************
   * Save LD_PRELOAD and clear it before running
   * xalt_run_submission.
   *********************************************************/

  p = getenv("LD_PRELOAD");
  if (p)
    ld_preload_strp = strdup(p);

  unsetenv("LD_PRELOAD");

  const char * blank = " ";

  char * env_path = getenv("PATH");
  if (!env_path)
    {
      pathArg = malloc(2);
      memcpy(pathArg,blank,2);
    }
  else
    {
      int ilen = 0;
      int plen = strlen(env_path);
      pathArg  = malloc(8 + plen + 2);
      memcpy(&pathArg[ilen], "--path \"", 8); ilen += 8;
      memcpy(&pathArg[ilen], env_path, plen); ilen += plen;
      memcpy(&pathArg[ilen], "\"", 2);
    }

  char * env_ldlibpath = getenv("LD_LIBRARY_PATH");
  if (!env_ldlibpath)
    {
      ldLibPathArg = malloc(2);
      memcpy(ldLibPathArg,blank,2);
    }
  else
    {
      int ilen     = 0;
      int plen     = strlen(env_ldlibpath);
      ldLibPathArg = malloc(14 + plen + 2);
      memcpy(&ldLibPathArg[ilen], "--ld_libpath \"", 14); ilen += 14;
      memcpy(&ldLibPathArg[ilen], env_ldlibpath, plen);   ilen += plen;
      memcpy(&ldLibPathArg[ilen], "\"", 2);
    }

  /* Push XALT_RUN_UUID, XALT_DATE_TIME into the environment so that things like
   * R and python can know what job and what start time of the this run is.
   */

  setenv("XALT_RUN_UUID",uuid_str,1);
  time_t time = start_time;
  strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));
  setenv("XALT_DATE_TIME",dateStr,1);

  ppid = getppid();
  asprintf(&cmdline, "LD_LIBRARY_PATH=%s PATH=/usr/bin:/bin %s --ppid %d --syshost \"%s\" --start \"%.3f\" --end 0 --exec \"%s\" --ntasks %ld"
	   " --uuid \"%s\" %s %s  '%s' %s", SYS_LD_LIB_PATH, PREFIX "/libexec/xalt_run_submission", ppid, my_syshost,
	   start_time, exec_path, my_size, uuid_str, pathArg, ldLibPathArg, usr_cmdline, (background ? "&":" "));

  DEBUG1(stderr, "  Recording state at beginning of user program:\n    %s\n\n}\n\n",cmdline);
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
  FILE * my_stderr = NULL;
  char * cmdline;

  if (xalt_tracing)
    {
      my_stderr = fdopen(errfd,"w");
      DEBUG1(my_stderr,"\nmyfini(%s){\n", STR(STATE));
    }

  /* Stop tracking if my mpi rank is not zero or the path was rejected. */
  if (reject_flag != XALT_SUCCESS)
    {
      DEBUG1(my_stderr,"    -> exiting because reject is set to: %s\n}\n\n", xalt_reasonA[reject_flag]);
      return;
    }

  end_time = epoch();
  unsetenv("LD_PRELOAD");

  /* Do not background this because it might get killed by the epilog cleanup tool! */

  asprintf(&cmdline, "LD_LIBRARY_PATH=%s PATH=/usr/bin:/bin %s --ppid %d --syshost \"%s\" --start \"%.3f\" --end \"%.3f\" --exec \"%s\""
           " --ntasks %ld --uuid \"%s\" %s %s '%s'", SYS_LD_LIB_PATH, PREFIX "/libexec/xalt_run_submission", ppid, my_syshost,
	   start_time, end_time, exec_path, my_size, uuid_str, pathArg, ldLibPathArg, usr_cmdline);

  DEBUG1(my_stderr,"  Recording State at end of user program:\n    %s\n\n}\n\n",cmdline);

  system(cmdline);
  free(cmdline);
  free(usr_cmdline);
  free(pathArg);
  free(ldLibPathArg);
}

xalt_status reject(const char *path, const char * hostname)
{
  int     i;
  regex_t regex;
  int     iret;
  int     rejected_host = (hostnameSz != 0);
  char    msgbuf[100];

  if (path[0] == '\0')
    return XALT_PATH;

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
      return XALT_HOSTNAME;
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
	  return XALT_SUCCESS;
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
	  return XALT_PATH;
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
  return XALT_SUCCESS;
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
	fprintf(stderr,"PID %ud: proc_pid();\n",getpid());
	fprintf(stderr,"    %s:\n", strerror(errno));
      }
  #else
    readlink("/proc/self/exe",path,sz-1);
  #endif
}

volatile double epoch()
{
  struct timeval tm;
  gettimeofday(&tm, 0);
  return tm.tv_sec + 1.0e-6*tm.tv_usec;
}


#ifdef __MACH__
  __attribute__((section("__DATA,__mod_init_func"), used, aligned(sizeof(void*)))) typeof(myinit) *__init = myinit;
  __attribute__((section("__DATA,__mod_term_func"), used, aligned(sizeof(void*)))) typeof(myfini) *__fini = myfini;
#else
  __attribute__((section(".init_array"))) typeof(myinit) *__init = myinit;
  __attribute__((section(".fini_array"))) typeof(myfini) *__fini = myfini;
#endif

