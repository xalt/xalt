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
#include "xalt_regex.h"
#include "xalt_interval.h"
#include <errno.h>
#include <sys/utsname.h>
#include <unistd.h>
#ifdef __MACH__
#  include <libproc.h>
#endif
#include "xalt_obfuscate.h"
#include "xalt_quotestring.h"
#include "xalt_config.h"
#include "xalt_fgets_alloc.h"
#include "xalt_path_parser.h"
#include "xalt_hostname_parser.h"
#include "build_uuid.h"

#ifdef HAVE_EXTERNAL_HOSTNAME_PARSER
#  define HOSTNAME_PARSER         my_hostname_parser
#  define HOSTNAME_PARSER_CLEANUP my_hostname_parser_cleanup
#else
#  define HOSTNAME_PARSER         hostname_parser
#  define HOSTNAME_PARSER_CLEANUP hostname_parser_cleanup
#endif


#define DATESZ    100

typedef enum { BIT_SCALAR = 1, BIT_SPSR = 2, BIT_MPI = 4, BIT_START_RECORD = 6} xalt_tracking_flags;
typedef enum { SPSR=1, KEEP=2, SKIP=3} xalt_parser;

typedef enum { XALT_SUCCESS = 0, XALT_TRACKING_OFF, XALT_WRONG_STATE, XALT_RUN_TWICE,
               XALT_MPI_RANK, XALT_HOSTNAME, XALT_PATH, XALT_BAD_JSON_STR, XALT_NO_OVERLAP,
	       XALT_SPSR_SAMPLING, XALT_MISSING_RUN_SUBMISSION} xalt_status;

static const char * xalt_reasonA[] = {
  "Successful XALT tracking",
  "XALT_EXECUTABLE_TRACKING is off",
  "__XALT_INITIAL_STATE__ is different from STATE",
  "XALT is trying to be twice in the same STATE",
  "XALT only tracks rank 0, in mpi programs",
  "XALT has found the host does not match hostname pattern",
  "XALT has found the executable does not match path pattern",
  "XALT has problem with a JSON string",
  "XALT execute type does not match requested type",
  "XALT SPSR sampling -> not recorded",
  "XALT Cannot find XALT_DIR/libexec/xalt_run_submission",
};


static const char * xalt_build_descriptA[] = {
  "track no programs at all",                     /* 0 */
  "track scalar programs only",                   /* 1 */
  "track SPSR programs only",                     /* 2 */
  "track SPSR and scalar programs only",          /* 3 */
  "track MPI programs only",                      /* 4 */
  "track MPI and scalar programs only",           /* 5 */
  "track MPI and SPSR programs only",             /* 6 */
  "track all programs"                            /* 7 */
};

static const char * xalt_run_descriptA[] = {
  "Not possible",                                 /* 0 */
  "program is a scalar program",                  /* 1 */
  "program is a SPSR program",                    /* 2 */
  "Not possible",                                 /* 3 */
  "program is an MPI program"                     /* 4 */
};

static const char * xalt_run_short_descriptA[] = {
  "Not possible",                                 /* 0 */
  "scalar",                                       /* 1 */
  "SPSR",                                         /* 2 */
  "Not possible",                                 /* 3 */
  "MPI"                                           /* 4 */
};


const  char*           xalt_syshost();
static long            compute_value(const char **envA);
static void            abspath(char * path, int sz);
static volatile double epoch();
static unsigned int    mix(unsigned int a, unsigned int b, unsigned int c); 
static double          scalar_program_sample_probability(double runtime);

void myinit(int argc, char **argv);
void myfini();

#define STR(x)   StR1_(x)
#define StR1_(x) #x

static int          countA[2];
static char         uuid_str[37];
static char         exec_path[PATH_MAX];
static char *       usr_cmdline;
static const char * my_syshost;


static int          run_submission_exists = -1;             /* 0 => does not exist; 1 => exists; -1 => status unknown */
static xalt_status  reject_flag	          = XALT_SUCCESS;
static int          run_mask              = 0;
static double       probability           = 1.0;
static pid_t        ppid	          = 0;
static int          errfd	          = -1;
static double       start_time	          = 0.0;
static double       end_time	          = 0.0;
static double       frac_time             = 0.0;
static long         my_rank	          = 0L;
static long         my_size	          = 1L;
static int          xalt_tracing          = 0;
static int          xalt_run_tracing      = 0;
static int          background	          = 0;
static char *       pathArg	          = NULL;
static char *       ldLibPathArg          = NULL;

#define HERE fprintf(stderr, "%s:%d\n",__FILE__,__LINE__)

#define DEBUG0(fp,s)             if (xalt_tracing) fprintf((fp),s)
#define DEBUG1(fp,s,x1)          if (xalt_tracing) fprintf((fp),s,(x1))
#define DEBUG2(fp,s,x1,x2)       if (xalt_tracing) fprintf((fp),s,(x1),(x2))
#define DEBUG3(fp,s,x1,x2,x3)    if (xalt_tracing) fprintf((fp),s,(x1),(x2),(x3))
#define DEBUG4(fp,s,x1,x2,x3,x4) if (xalt_tracing) fprintf((fp),s,(x1),(x2),(x3),(x4))

void myinit(int argc, char **argv)
{
  int    i;
  int    produce_strt_rec = 0;
  char * p;
  char * p_dbg;
  char * cmdline;
  char * ld_preload_strp = NULL;
  char   dateStr[DATESZ];
  char   fullDateStr[DATESZ];
  const char * v;
  xalt_parser  results;
  xalt_parser  path_results;

  /* The SLURM env's must be last.  On Stampede2 both PMI_RANK and SLURM_PROCID are set. Only PMI_RANK is correct with multiple ibrun -n -o */
  /* Lonestar 5, Cray XC-40, only has SLURM_PROCID */
  const char * rankA[] = {"PMI_RANK", "OMPI_COMM_WORLD_RANK", "MV2_COMM_WORLD_RANK", "SLURM_PROCID",         NULL };
  const char * sizeA[] = {"PMI_SIZE", "OMPI_COMM_WORLD_SIZE", "MV2_COMM_WORLD_SIZE", "SLURM_STEP_NUM_TASKS", NULL };

  struct utsname u;

  /* Check to see if backgrounding should be turned on*/
  v = getenv("XALT_ENABLE_BACKGROUNDING");
  if (v == NULL)
    v = XALT_ENABLE_BACKGROUNDING;
  
  if (strcmp(v,"yes") == 0)
    background = 1;

  p_dbg = getenv("XALT_TRACING");
  if (p_dbg)
    {
      background       = 0;  /* backgrounding the start record is always off when tracing is on*/
      xalt_tracing     = (strcmp(p_dbg,"yes") == 0);
      xalt_run_tracing = (strcmp(p_dbg,"run") == 0);
      if (xalt_tracing  || xalt_run_tracing)
	errfd	       = dup(STDERR_FILENO);
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

  /***********************************************************
   * Test 0a: Initial State Test?:
   * Is this is built-in or LD_PRELOAD version of this file?
   ***********************************************************/

  DEBUG2(stderr,"  Test for __XALT_INITIAL_STATE__: \"%s\", STATE: \"%s\"\n", (v != NULL) ? v : "(NULL)", STR(STATE));
  /* Stop tracking if any myinit routine has been called */
  if (v && (strcmp(v,STR(STATE)) != 0))
    {
      DEBUG2(stderr,"    -> __XALT_INITIAL_STATE__ has a value: \"%s\" -> and it is different from STATE: \"%s\" exiting\n}\n\n",v,STR(STATE));
      reject_flag = XALT_WRONG_STATE;
      return;
    }

  /***********************************************************
   * Test 0b: Count test?:
   * Is the same version of this file run more than once?
   ***********************************************************/
  if (countA[IDX] > 0)
    {
      DEBUG2(stderr,"    -> countA[%d]: %d which is greater than 0 -> exiting\n}\n\n",IDX,countA[IDX]);
      reject_flag = XALT_RUN_TWICE;
      return;
    }
  countA[IDX]++;

  /***********************************************************
   * Test 1: XALT_EXECUTABLE_TRACKING?:
   * Is xalt tracking turned on?
   ***********************************************************/

  /* Stop tracking if XALT is turned off */
  v = getenv("XALT_EXECUTABLE_TRACKING");
  DEBUG1(stderr,"  Test for XALT_EXECUTABLE_TRACKING: %s\n",(v != NULL) ? v : "(NULL)");
  if (!v || strcmp(v,"yes") != 0)
    {
      DEBUG0(stderr,"    -> XALT_EXECUTABLE_TRACKING is off -> exiting\n}\n\n");
      reject_flag = XALT_TRACKING_OFF;
      unsetenv("XALT_RUN_UUID");
      return;
    }


  /***********************************************************
   * Test 2: MPI Rank > 0?:
   * Stop tracking if my mpi rank is not zero
   ***********************************************************/

  my_rank = compute_value(rankA);
  DEBUG1(stderr,"  Test for rank == 0, rank: %ld\n",my_rank);
  if (my_rank > 0L)
    {
      DEBUG0(stderr,"    -> MPI Rank is not zero -> exiting\n}\n\n");
      reject_flag = XALT_MPI_RANK;
      unsetenv("XALT_RUN_UUID");
      return;
    }

  /***********************************************************
   * Test 3: Test for acceptable hostname:
   ***********************************************************/
  errno = 0;
  if (uname(&u) != 0)
    {
      perror("uname");
      exit(EXIT_FAILURE);
    }

  results = HOSTNAME_PARSER(u.nodename);
  HOSTNAME_PARSER_CLEANUP();
  if (results == SKIP)
    {
      DEBUG1(stderr,"    hostname: \"%s\" is rejected\n",u.nodename);
      reject_flag = XALT_HOSTNAME;
      unsetenv("XALT_RUN_UUID");
      return; 
    }

  /***********************************************************
   * Test 4: Acceptable Executable && MPI status?
   ***********************************************************/

  int build_mask = 0;
  v = getenv("XALT_SCALAR_TRACKING");
  if (!v)
    v = XALT_SCALAR_TRACKING;
  if (strcasecmp(v,"yes") == 0)
    build_mask |= BIT_SCALAR;

  v = getenv("XALT_SPSR_TRACKING");
  if (!v)
    v = XALT_SPSR_TRACKING;
  if (strcasecmp(v,"yes") == 0)
    build_mask |= BIT_SPSR;
      
  v = getenv("XALT_MPI_TRACKING");
  if (!v)
    v = XALT_MPI_TRACKING;
  if (strcasecmp(v,"yes") == 0)
    build_mask |= BIT_MPI;
      
  
  /* Test for MPI-ness */
  my_size      = compute_value(sizeA);
  if (my_size < 1L)
    my_size = 1L;

  /* Get full absolute path to executable */
  abspath(exec_path,sizeof(exec_path));
  path_results = keep_path(exec_path);
  path_parser_cleanup();
  
  if (path_results == SKIP)
    {
      DEBUG1(stderr,"    executable: \"%s\" is rejected\n", exec_path);
      reject_flag = XALT_PATH;
      unsetenv("XALT_RUN_UUID");
      return;
    }

  if (my_size > 1L)
    run_mask |= BIT_MPI;
  else
    run_mask |= (path_results == SPSR) ? BIT_SPSR : BIT_SCALAR;
                      
  if (build_mask & run_mask & BIT_SPSR)
    {
      const char * v;
      v = getenv("XALT_SCALAR_AND_SPSR_SAMPLING");
      if (v && strcmp(v,"yes") == 0)
	{
	  const char* xalt_spsr_sampling_rate_str = getenv("XALT_SPSR_SAMPLING_RATE");
	  if (xalt_spsr_sampling_rate_str)
	    spsr_sampling_rate = strtod(xalt_spsr_sampling_rate_str, NULL);
	  
	  unsigned int a	= (unsigned int) clock();
	  unsigned int b	= (unsigned int) time(NULL);
	  unsigned int c	= (unsigned int) getpid();
	  unsigned int seed	= mix(a,b,c);
	  srand(seed);
	  double my_rand        = (double) rand()/(double) RAND_MAX;

	  if (my_rand >= spsr_sampling_rate)
	    {
	      DEBUG3(stderr, "    -> exiting because spsr sampling. "
		     "(my_rand: %g >= SPSR sampling: %g) for program: %s\n}\n\n",
		     my_rand, spsr_sampling_rate, exec_path);
	      reject_flag = XALT_SPSR_SAMPLING;
	      unsetenv("XALT_RUN_UUID");
	      return;
	    }
	  else
	    DEBUG3(stderr, "    -> spsr sampling. "
		     "(my_rand: %g < SPSR sampling: %g) for program: %s\n}\n\n",
		     my_rand, spsr_sampling_rate, exec_path);
	}
    }

  /* Test for an acceptable executable */
  if ((build_mask & run_mask) == 0)
    {
      DEBUG2(stderr,"    -> XALT is build to %s, Current %s -> not tracking and exiting\n}\n\n",
             xalt_build_descriptA[build_mask], xalt_run_descriptA[run_mask]);
      reject_flag = XALT_NO_OVERLAP;
      unsetenv("XALT_RUN_UUID");
      return;
    }

  setenv("__XALT_INITIAL_STATE__",STR(STATE),1);

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
  for (i = 0; i < argc; ++i)
    {
      *p++ = '"';
      const char* qs  = xalt_quotestring(argv[i]);
      int         len = strlen(qs);
      memcpy(p,qs,len);
      p += len;
      *p++= '"';
      *p++= ' ';
    }
  *--p = '\0';
  if (p > &usr_cmdline[sz])
    {
      fprintf(stderr,"XALT: Failure in building user command line json string!\n");
      reject_flag = XALT_BAD_JSON_STR;
      unsetenv("XALT_RUN_UUID");
      return;
    }

  xalt_quotestring_free();
  
  build_uuid(uuid_str);
  start_time = epoch();
  frac_time  = start_time - (long) (start_time);

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
      pathArg = (char *) malloc(2);
      memcpy(pathArg,blank,2);
    }
  else
    {
      int ilen = 0;
      int plen = strlen(env_path);
      pathArg  = (char *) malloc(8 + plen + 2);
      memcpy(&pathArg[ilen], "--path \"", 8); ilen += 8;
      memcpy(&pathArg[ilen], env_path, plen); ilen += plen;
      memcpy(&pathArg[ilen], "\"", 2);
    }

  char * env_ldlibpath = getenv("LD_LIBRARY_PATH");
  if (!env_ldlibpath)
    {
      ldLibPathArg = (char *) malloc(2);
      memcpy(ldLibPathArg,blank,2);
    }
  else
    {
      int ilen     = 0;
      int plen     = strlen(env_ldlibpath);
      ldLibPathArg = (char *) malloc(14 + plen + 2);
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
  sprintf(fullDateStr,"%s_%d",dateStr, (int) (frac_time*10000.0));

  setenv("XALT_DATE_TIME",fullDateStr,1);
  setenv("XALT_DIR",XALT_DIR,1);

  ppid = getppid();

  /* 
   * XALT is only recording the end record for scalar executables and
   * not the start record.  The only exception to this is when the exec_path
   * matches one of the patterns in path_patterns returns SPSR.
   */

  if ( run_mask & BIT_START_RECORD)  /* BIT_START_RECORD = BIT_SPSR | BIT_MPI */
    {
      const char * run_submission = XALT_DIR "/libexec/xalt_run_submission";
      int runable = access(run_submission, X_OK);

      if (runable == -1)
        {
          run_submission_exists = 0;
          DEBUG1(stderr, "    -> Quitting => Cannot find xalt_run_submission: %s\n}\n\n", run_submission);
          reject_flag = XALT_MISSING_RUN_SUBMISSION;
          unsetenv("XALT_RUN_UUID");
          return;
        }
      run_submission_exists = 1;

      asprintf(&cmdline, "LD_LIBRARY_PATH=%s PATH=/usr/bin:/bin %s --interfaceV %s --ppid %d --syshost \"%s\" --start \"%.4f\" --end 0 --exec \"%s\" --ntasks %ld"
	       " --uuid \"%s\" --prob %g %s %s -- %s %s", CXX_LD_LIBRARY_PATH, run_submission, XALT_INTERFACE_VERSION, ppid, my_syshost,
	       start_time, exec_path, my_size, uuid_str, probability, pathArg, ldLibPathArg, usr_cmdline, (background ? "&":" "));

      if (xalt_tracing || xalt_run_tracing) 
	fprintf(stderr, "  Recording state at beginning of %s user program:\n    %s\n\n}\n\n",
		xalt_run_short_descriptA[run_mask], cmdline);
      system(cmdline);
      free(cmdline);
    }
  else
    {
      DEBUG2(stderr,"    -> XALT is build to %s, Current %s -> Not producing a start record\n}\n\n",
             xalt_build_descriptA[build_mask], xalt_run_descriptA[run_mask]);
    }
      



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
  double run_time;
  int    xalt_err = xalt_tracing || xalt_run_tracing;

  if (xalt_err)
    {
      close(STDERR_FILENO);
      my_stderr = fdopen(errfd,"w");
      DEBUG1(my_stderr,"\nmyfini(%s){\n", STR(STATE));
    }

  /* Stop tracking if my mpi rank is not zero or the path was rejected. */
  if (reject_flag != XALT_SUCCESS)
    {
      DEBUG2(my_stderr,"    -> exiting because reject is set to: %s for program: %s\n}\n\n",
	     xalt_reasonA[reject_flag], exec_path);
      if (xalt_err)
	{
	  fclose(my_stderr);
	  close(errfd);
	}
      return;
    }

  end_time = epoch();
  unsetenv("LD_PRELOAD");

  if (run_mask & BIT_SCALAR)
    {
      const char * v;
      v = getenv("XALT_SCALAR_AND_SPSR_SAMPLING");
      if (v && strcmp(v,"yes") == 0)
	{
	  double       run_time	= end_time - start_time;
	  probability           = scalar_program_sample_probability(run_time);
	  unsigned int a	= (unsigned int) clock();
	  unsigned int b	= (unsigned int) time(NULL);
	  unsigned int c	= (unsigned int) getpid();
	  unsigned int seed	= mix(a,b,c);

	  srand(seed);
	  double my_rand    = (double) rand()/(double) RAND_MAX;

	  if (my_rand >= probability)
	    {
	      DEBUG4(my_stderr, "    -> exiting because scalar sampling. "
		     "run_time: %g, (my_rand: %g > prob: %g) for program: %s\n}\n\n",
		     run_time, my_rand, probability, exec_path);
	      if (xalt_err) 
		{
		  fclose(my_stderr);
		  close(errfd);
		}
	      return;
	    }
	  else
	    DEBUG4(my_stderr, "    -> Scalar Sampling program run_time: %g: (my_rand: %g <= prob: %g) for program: %s\n", 
		   run_time, my_rand, probability, exec_path);
	}
    }
  
  const char * run_submission = XALT_DIR "/libexec/xalt_run_submission";
  int runable = (run_submission_exists == 1) ? 1 : access(run_submission, X_OK);
  if (runable == -1)
    {
      DEBUG1(my_stderr, "    -> Quitting => Cannot find xalt_run_submission: %s\n}\n\n", run_submission);
      reject_flag = XALT_MISSING_RUN_SUBMISSION;
    }
  else
    {

      /* Do not background this because it might get killed by the epilog cleanup tool! */

      asprintf(&cmdline, "LD_LIBRARY_PATH=%s PATH=/usr/bin:/bin %s --interfaceV %s --ppid %d --syshost \"%s\" --start \"%.4f\" --end \"%.4f\" --exec \"%s\""
               " --ntasks %ld --uuid \"%s\" --prob %g %s %s -- %s", CXX_LD_LIBRARY_PATH, run_submission, XALT_INTERFACE_VERSION, ppid, my_syshost,
               start_time, end_time, exec_path, my_size, uuid_str, probability, pathArg, ldLibPathArg, usr_cmdline);

      if (xalt_tracing || xalt_run_tracing )
        fprintf(my_stderr,"  Recording State at end of %s user program:\n    %s\n}\n\n",
                xalt_run_short_descriptA[run_mask], cmdline);

      system(cmdline);
      free(cmdline);
    }
  free(usr_cmdline);
  free(pathArg);
  free(ldLibPathArg);
  if (xalt_err) 
    {
      fclose(my_stderr);
      close(errfd);
    }
}


static long compute_value(const char **envA)
{
  long          value = 0L;
  const char ** p;
  for (p = &envA[0]; *p; ++p)
    {
      char *v = getenv(*p);
      if (v)
	{
	  value += strtol(v, (char **) NULL, 10);
	  break;
	}
    }
  return value;
}

/* Get full absolute path to executable */
/* works for Linux and Mac OS X */
static void abspath(char * path, int sz)
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

static volatile double epoch()
{
  struct timeval tm;
  gettimeofday(&tm, 0);
  return tm.tv_sec + 1.0e-6*tm.tv_usec;
}

static unsigned int mix(unsigned int a, unsigned int b, unsigned int c)
{
  a=a-b;  a=a-c;  a=a^(c >> 13);
  b=b-c;  b=b-a;  b=b^(a << 8);
  c=c-a;  c=c-b;  c=c^(b >> 13);
  a=a-b;  a=a-c;  a=a^(c >> 12);
  b=b-c;  b=b-a;  b=b^(a << 16);
  c=c-a;  c=c-b;  c=c^(b >> 5);
  a=a-b;  a=a-c;  a=a^(c >> 3);
  b=b-c;  b=b-a;  b=b^(a << 10);
  c=c-a;  c=c-b;  c=c^(b >> 15);
  return c;
}

static  double scalar_program_sample_probability(double runtime)
{
  double prob = 1.0;
  int i;
  for (i = 0; i < rangeSz-1; ++i)
    {
      if (runtime < rangeA[i+1].left)
  	{
  	  prob = rangeA[i].prob;
  	  break;
  	}
    }
  return prob;
}  


#ifdef __MACH__
  __attribute__((section("__DATA,__mod_init_func"), used, aligned(sizeof(void*)))) __typeof__(myinit) *__init = myinit;
  __attribute__((section("__DATA,__mod_term_func"), used, aligned(sizeof(void*)))) __typeof__(myfini) *__fini = myfini;
#else
  __attribute__((section(".init_array"))) __typeof__(myinit) *__init = myinit;
  __attribute__((section(".fini_array"))) __typeof__(myfini) *__fini = myfini;
#endif
