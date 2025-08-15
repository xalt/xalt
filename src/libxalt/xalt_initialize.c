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
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include "xalt_interval.h"
#include <link.h>
#include <errno.h>
#include <syslog.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __MACH__
#  include <libproc.h>
#endif
#include "xalt_obfuscate.h"
#include "xalt_types.h"
#include "xalt_timer.h"
#include "insert.h"
#include "base64.h"
#include "xalt_quotestring.h"
#include "xalt_header.h"
#include "xalt_config.h"
#include "xalt_dir.h"
#include "xalt_hostname_parser.h"
#include "xalt_tmpdir.h"
#include "xalt_vendor_note.h"
#include "buildXALTRecordT.h"
#include "build_uuid.h"
#include "xalt_c_utils.h"
#include "run_submission.h"
#include "xalt_track_executable.h"

#if USE_DCGM && USE_NVML
#error "Both DCGM and NVML enabled.  This is not allowed."
#endif

#ifdef USE_NVML
/* This code will only ever be active in 64 bit mode and not 32 bit mode */
#include <dlfcn.h>
#include <nvml.h>
#endif

#ifdef USE_DCGM
/* This code will only ever be active in 64 bit mode and not 32 bit mode */
#  include <dlfcn.h>
#  include <dcgm_agent.h>
#  include <dcgm_structs.h>
#endif

#ifdef HAVE_EXTERNAL_HOSTNAME_PARSER
#  define HOSTNAME_PARSER         my_hostname_parser
#  define HOSTNAME_PARSER_CLEANUP my_hostname_parser_cleanup
#else
#  define HOSTNAME_PARSER         hostname_parser
#  define HOSTNAME_PARSER_CLEANUP hostname_parser_cleanup
#endif


#define DATESZ      40
#define FULLDATESZ  2*DATESZ
#define N_ELEMENTS(a) (sizeof(a)/sizeof((a)[0]))

typedef enum { BIT_SCALAR = 1, BIT_PKGS = 2, BIT_MPI = 4} xalt_tracking_flags;

typedef enum { XALT_SUCCESS = 0, XALT_TRACKING_OFF, XALT_WRONG_STATE, XALT_RUN_TWICE,
               XALT_MPI_RANK, XALT_HOSTNAME, XALT_PATH, XALT_BAD_JSON_STR, XALT_NO_OVERLAP,
               XALT_UNAME_FAILURE, XALT_NO_UUID}  xalt_status;

static const char * xalt_reasonA[] = {
  /*  0 */ "Successful XALT tracking",
  /*  1 */ "XALT_EXECUTABLE_TRACKING is off",
  /*  2 */ "__XALT_INITIAL_STATE__ is different from STATE",
  /*  3 */ "XALT is trying to be twice in the same STATE",
  /*  4 */ "XALT only tracks rank 0, in mpi programs",
  /*  5 */ "XALT has found the host does not match hostname pattern. If this is unexpected check your config.py file: "   XALT_CONFIG_PY ,
  /*  6 */ "XALT has found the executable does not match path pattern. If this is unexpected check your config.py file: " XALT_CONFIG_PY ,
  /*  7 */ "XALT has problem with a JSON string",
  /*  8 */ "XALT execute type does not match requested type",
  /*  9 */ "XALT Cannot find XALT_DIR/libexec/xalt_run_submission",
  /* 10 */ "XALT Cannot produce Run UUID", 
};


static const char * xalt_build_descriptA[] = {
  "track no programs at all",                     /* 0 */
  "track scalar programs only",                   /* 1 */
  "Not possible (2)",                             /* 2 */
  "Not possible (3)",                             /* 3 */
  "track MPI programs only",                      /* 4 */
  "track all programs",                           /* 5 */
  "Not possible (6)",                             /* 6 */
  "Not possible (7)",                             /* 7 */
};

static const char * xalt_run_descriptA[] = {
  "Not possible (0)",                             /* 0 */
  "program is a scalar program",                  /* 1 */
  "Not possible (2)",                             /* 2 */
  "Not possible (3)",                             /* 3 */
  "program is an MPI program"                     /* 4 */
};

static const char * xalt_run_short_descriptA[] = {
  "Not possible (0)",                             /* 0 */
  "scalar",                                       /* 1 */
  "PKGS",                                         /* 2 */
  "Not possible (3)",                             /* 3 */
  "MPI"                                           /* 4 */
};

static long            compute_value(const char **envA);
static void            get_abspath(char * path, int sz);
static volatile double epoch();
static unsigned int    mix(unsigned int a, unsigned int b, unsigned int c);
static double          prgm_sample_probability(int ntasks, double runtime);
#ifdef USE_NVML
static int             load_nvml();
#endif
#ifdef USE_DCGM
static int             load_dcgm();
#endif

void myinit(int argc, char **argv);
void myfini();
void wrapper_for_myfini(int signum);
#define STR(x)   StR1_(x)
#define StR1_(x) #x

#define BUFSZ       100
static xalt_timer_t xalt_timer;
static int          countA[2];
static char         buffer[BUFSZ];
static char         uuid_str[37];
static char         exec_path[PATH_MAX+1];
static char *       usr_cmdline;

static char *       watermark             = NULL;
static xalt_status  reject_flag           = XALT_SUCCESS;
static int          run_mask              = 0;
static int          have_uuid             = 0;
static long         always_record         = 0L;
static double       probability           = 1.0;
static double       testing_runtime       = -1.0;
static int          orig_pid              = 0;
static pid_t        ppid                  = 0;
static int          errfd                 = -1;
static double       my_rand               = 0.0;
static double       start_time            = 0.0;
static double       end_time              = 0.0;
static double       frac_time             = 0.0;
static long         my_rank               = 0L;
static long         my_size               = 1L;
static int          num_tasks             = 0;
static int          xalt_kind             = 0;
static int          xalt_tracing          = 0;
static int          xalt_run_tracing      = 0;
static int          xalt_gpu_tracking     = 0;
static int          xalt_sampling         = 0;
static int          num_gpus              = 0;
static int          b64_len               = 0;
static int          b64_wm_len            = 0;
static int          signal_hdlr_called    = 0;
#ifdef USE_NVML
static unsigned long long __time          = 0;
static void * nvml_dl_handle              = NULL;
static nvmlReturn_t (*_nvmlDeviceGetAccountingBufferSize)(nvmlDevice_t,
                                                          unsigned int *);
static nvmlReturn_t (*_nvmlDeviceGetAccountingMode)(nvmlDevice_t,
                                                    nvmlEnableState_t *);
static nvmlReturn_t (*_nvmlDeviceGetAccountingPids)(nvmlDevice_t,
                                                    unsigned int *,
                                                    unsigned int *);
static nvmlReturn_t (*_nvmlDeviceGetAccountingStats)(nvmlDevice_t,
                                                     unsigned int,
                                                     nvmlAccountingStats_t *);
static nvmlReturn_t (*_nvmlDeviceGetCount)(unsigned int *);
static nvmlReturn_t (*_nvmlDeviceGetHandleByIndex)(unsigned int,
                                                   nvmlDevice_t *);
static char * (*_nvmlErrorString)(nvmlReturn_t);
static nvmlReturn_t (*_nvmlInit)();
static nvmlReturn_t (*_nvmlShutdown)();
#endif
#ifdef USE_DCGM
static void *       dcgm_dl_handle        = NULL;
static dcgmHandle_t dcgm_handle           = NULL;
/* This code will only every be active in 64 bit mode and not 32 bit mode*/
/* Temporarily disable any stderr messages from DCGM */
#define DCGMFUNC2(FUNC,x1,x2,out)    \
{                                    \
  dcgmReturn_t __result;             \
  int fd1, fd2;                      \
  fflush(stderr);                    \
  fd1 = dup(STDERR_FILENO);          \
  fd2 = open("/dev/null", O_WRONLY); \
  dup2(fd2, STDERR_FILENO);          \
  close(fd2);                        \
  __result = FUNC(x1, x2);           \
  fflush(stderr);                    \
  dup2(fd1, STDERR_FILENO);          \
  close(fd1);                        \
  (*(out)) = __result;               \
}

static dcgmReturn_t (*_dcgmInit)(void);
static dcgmReturn_t (*_dcgmShutdown)(void);
static dcgmReturn_t (*_dcgmStartEmbedded)(dcgmOperationMode_t opMode, dcgmHandle_t *pDcgmHandle);
static dcgmReturn_t (*_dcgmJobStartStats)(dcgmHandle_t pDcgmHandle, dcgmGpuGrp_t groupId, char jobId[64]);
static dcgmReturn_t (*_dcgmJobGetStats)(dcgmHandle_t pDcgmHandle, char jobId[64], dcgmJobInfo_t *pJobInfo);
static dcgmReturn_t (*_dcgmJobRemove)(dcgmHandle_t pDcgmHandle, char jobId[64]);
static dcgmReturn_t (*_dcgmJobStopStats)(dcgmHandle_t pDcgmHandle, char jobId[64]);
static dcgmReturn_t (*_dcgmStopEmbedded)(dcgmHandle_t pDcgmHandle);
static dcgmReturn_t (*_dcgmUpdateAllFields)(dcgmHandle_t pDcgmHandle, int waitForUpdate);
static dcgmReturn_t (*_dcgmWatchJobFields)(dcgmHandle_t pDcgmHandle,
                                           dcgmGpuGrp_t groupId,
                                           long long updateFreq,
                                           double maxKeepAge,
                                           int maxKeepSamples);

#endif

extern char **environ;

void myinit(int argc, char **argv)
{
  int    i;
  char * p;
  char * p_dbg;
  int    produce_strt_rec = 0;
  int    debugMe          = 0;
  char * cmdline          = NULL;
  char * ld_preload_strp  = NULL;
  char * pid_str          = NULL;
  char   dateStr[DATESZ];
  char   fullDateStr[FULLDATESZ];
  const char * v;
  xalt_parser  results;
  xalt_parser  path_results;
  const char   nvidia_dir[] = "/sys/module/nvidia";

  /* The SLURM env's must be last.  On Stampede2 both PMI_RANK and SLURM_PROCID are set. Only PMI_RANK is correct with multiple ibrun -n -o */
  /* Lonestar 5, Cray XC-40, only has SLURM_PROCID */
  const char * sizeA[] = {"XALT_COMM_WORLD_SIZE",
                          "OMPI_COMM_WORLD_SIZE", "MV2_COMM_WORLD_SIZE", "PMI_SIZE", "SLURM_STEP_NUM_TASKS", NULL };                          
  const char * rankA[] = {"OMPI_COMM_WORLD_RANK", "MV2_COMM_WORLD_RANK", "PMI_RANK", "SLURM_PROCID",         NULL };


  struct utsname u;

  orig_pid             = getpid();
  double t0            = epoch();
  xalt_timer.init      = 0.0;
  xalt_timer.fini      = 0.0;
  xalt_timer.gpu_setup = 0.0;
  my_rank = compute_value(rankA);

  p_dbg = getenv("XALT_TRACING");
  if (p_dbg)
    {
      xalt_tracing     = (strcmp( p_dbg,"yes")   == 0);
      xalt_run_tracing = (strncmp(p_dbg,"run",3) == 0);
      if (my_rank == 0 && ((xalt_tracing | xalt_run_tracing) == 0))
        xalt_tracing     = (strcmp(p_dbg,"yes0") == 0);

      if (xalt_tracing  || xalt_run_tracing)
        errfd          = dup(STDERR_FILENO);
    }

  if (xalt_run_tracing && my_rank == 0)
    {
      debugMe = 1;
      get_abspath(exec_path,sizeof(exec_path));
      path_results = track_executable(debugMe, exec_path, argc, argv);
      xalt_tracing = (path_results != SKIP);
    }

  if (xalt_tracing)
    {
      time_t    now    = (time_t) t0;
      strftime(dateStr, DATESZ, "%c", localtime(&now));
      errno = 0;
      if (uname(&u) != 0)
        {
          reject_flag = XALT_UNAME_FAILURE;
          DEBUG(stderr, "    -> uname had a failure -> exiting\n}\n\n");
          return;
        }

      fprintf(stderr, "---------------------------------------------\n"
                      " Date:          %s\n"
                      " XALT Version:  %s\n"
                      " Nodename:      %s\n"
                      " System:        %s\n"
                      " Release:       %s\n"
                      " O.S. Version:  %s\n"
                      " Machine:       %s\n"
                      "---------------------------------------------\n\n",
              dateStr, XALT_GIT_VERSION, u.nodename, u.sysname, u.release,
              u.version, u.machine);
      my_size = compute_value(sizeA);
      if (my_size < 1L)
        my_size = 1L;
      num_tasks = (int) my_size;
      get_abspath(exec_path,sizeof(exec_path));
      fprintf(stderr,"myinit(%ld/%d,%s,%s){\n", my_rank, num_tasks, STR(STATE),exec_path);
    }

  /***********************************************************
   * Test 0a: Initial State Test?:
   * Is this is built-in or LD_PRELOAD version of this file?
   ***********************************************************/

  v = getenv("__XALT_INITIAL_STATE__");
  DEBUG(stderr,"  Test for __XALT_INITIAL_STATE__: \"%s\", STATE: \"%s\"\n", (v != NULL) ? v : "(NULL)", STR(STATE));
  /* Stop tracking if another myinit() routine has been called with my pid and hostname*/
  if (v && (strcmp(v,STR(STATE)) != 0))
    {
      if (uname(&u) != 0)
        {
          DEBUG(stderr, "    -> uname had a failure -> exiting\n}\n\n");
          reject_flag = XALT_UNAME_FAILURE;
          return;
        }
      asprintf(&pid_str,"%d:%s", orig_pid, u.nodename);
      char* env_pid = getenv("__XALT_STATE_PID__");
      if (env_pid && strcmp(env_pid,pid_str) == 0)
        {
          DEBUG(stderr,"    -> __XALT_INITIAL_STATE__ has a value: \"%s\" -> and it is different from STATE: \"%s\" and PID's match: %s -> exiting\n}\n\n",v, STR(STATE), env_pid);
          reject_flag = XALT_WRONG_STATE;
          return;
        }
    }

  /***********************************************************
   * Test 0b: Count test?:
   * Is the same version of this file run more than once?
   ***********************************************************/
  if (countA[IDX] > 0)
    {
      DEBUG(stderr,"    -> countA[%d]: %d which is greater than 0 -> exiting\n}\n\n",IDX,countA[IDX]);
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
  DEBUG(stderr,"  Test for XALT_EXECUTABLE_TRACKING: %s\n",(v != NULL) ? v : "(NULL)");
  if (!v || strcmp(v,"yes") != 0)
    {
      DEBUG(stderr,"    -> XALT_EXECUTABLE_TRACKING is off -> exiting\n}\n\n");
      reject_flag = XALT_TRACKING_OFF;
      unsetenv("XALT_RUN_UUID");
      return;
    }


  /***********************************************************
   * Test 2: MPI Rank > 0?:
   * Stop tracking if my mpi rank is not zero
   ***********************************************************/

  DEBUG(stderr,"  Test for rank == 0, rank: %ld\n",my_rank);
  if (my_rank > 0L)
    {
      DEBUG(stderr,"    -> MPI Rank is not zero -> exiting\n}\n\n");
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
      DEBUG(stderr,"    hostname: \"%s\" is rejected -> exiting\n}\n\n",u.nodename);
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

  v = getenv("XALT_MPI_TRACKING");
  if (!v)
    v = XALT_MPI_TRACKING;
  if (strcasecmp(v,"yes") == 0)
    build_mask |= BIT_MPI;


  /* Get full absolute path to executable */
  get_abspath(exec_path, sizeof(exec_path));
  debugMe = 0;
  path_results = track_executable(debugMe, exec_path, argc, argv);

  if (path_results == SKIP)
    {
      DEBUG(stderr,"    executable: \"%s\" is rejected  -> exiting\n}\n\n", exec_path);
      reject_flag = XALT_PATH;
      unsetenv("XALT_RUN_UUID");
      return;
    }

  my_size = compute_value(sizeA);
  if (my_size < 1L)
    my_size = 1L;
  num_tasks = (int) my_size;

  if (num_tasks > 1)
    {
      run_mask |= BIT_MPI;
      xalt_kind = BIT_MPI;
    }
  else
    {
      run_mask |= BIT_SCALAR;
      xalt_kind = BIT_SCALAR;
    }

  if (path_results == PKGS)
    xalt_kind   = BIT_PKGS;


  /* Test for an acceptable executable */
  if ((build_mask & run_mask) == 0)
    {
      DEBUG(stderr,"    -> XALT is build to %s, Current %s -> not tracking and exiting\n}\n\n",
             xalt_build_descriptA[build_mask], xalt_run_descriptA[run_mask]);
      reject_flag = XALT_NO_OVERLAP;
      unsetenv("XALT_RUN_UUID");
      return;
    }

  if (pid_str == NULL)
    asprintf(&pid_str,"%ld:%s",(long) orig_pid, u.nodename);

  setenv("__XALT_INITIAL_STATE__",    STR(STATE),1);
  setenv("__XALT_STATE_PID__",        pid_str,1);
  my_free(pid_str,strlen(pid_str));

  /* Build a json version of the user's command line. */

  /* Calculate size of buffer*/
  int sz = 0;
  for (i = 0; i < argc; ++i)
    sz += strlen(argv[i]);

  /* this size formula uses 3 facts:
   *   1) if every character was a utf-16 character that is four bytes converts to 12 (sz*3)
   *   2) Each entry has two quotes and a space (argc*3)
   *   3) There are a pair of square brackets and a null byte (+3)
   */

  sz = sz*3 + argc*3 + 3;

  usr_cmdline = (char *) XMALLOC(sz);
  p           = &usr_cmdline[0];
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

  int qsLen   = p - usr_cmdline;

  if (p > &usr_cmdline[sz])
    {
      fprintf(stderr,"XALT: Failure in building user command line json string! -> exiting\n}\n\n");
      reject_flag = XALT_BAD_JSON_STR;
      unsetenv("XALT_RUN_UUID");
      return;
    }
  xalt_quotestring_free();

#if USE_DCGM || USE_NVML
  /* This code will only ever be active in 64 bit mode and not 32 bit mode */
  double t_gpu = epoch();
  v  = getenv("XALT_GPU_TRACKING");
  if (v == NULL)
    v = XALT_GPU_TRACKING;
  xalt_gpu_tracking = (strcasecmp(v,"yes") == 0);
  do
    {
      if (xalt_gpu_tracking)
        {

          struct stat s = {0};
          /* check whether the nvidia module is loaded */
          if (stat(nvidia_dir, &s) !=  0 || ! S_ISDIR(s.st_mode))
            {
              xalt_gpu_tracking = 0;
              DEBUG(stderr, "  GPU tracing is turned off. This directory \"%s\" does not exist!\n", nvidia_dir);
              break;
            }
#ifdef USE_NVML
          DEBUG(stderr, "  GPU tracing with NVML\n");
          /* Open the NVML library at runtime.  This avoids failing if
             the library is not available on a particular system.  In
             that case, the handle will not be created and GPU
             tracking will be disabled. */
          if (load_nvml() == 0) {
            xalt_gpu_tracking = 0;
            break;
          }

          nvmlReturn_t result;
          struct timeval tv;

          /* Mark time the program started.  Use this to compare to
           * GPU process timestamps in fini to only count processes
           * that started during the lifetime of the program. */
          gettimeofday(&tv, NULL);
          __time = (unsigned long long)(tv.tv_sec) * 1000000 + (unsigned long long)(tv.tv_usec);

          result = _nvmlInit();
          if (result != NVML_SUCCESS)
            {
              DEBUG(stderr, "    -> Stopping GPU Tracking => Cannot initialize NVML: %s\n\n", _nvmlErrorString(result));
              xalt_gpu_tracking = 0;
              break;
            }
#elif USE_DCGM
          DEBUG(stderr, "  GPU tracing with DCGM\n");

          /* Open the DCGM library at runtime.  This avoids failing if
             the library is not available on a particular system.  In
             that case, the handle will not be created and GPU
             tracking will be disabled. */
          if (load_dcgm() == 0) {
            xalt_gpu_tracking = 0;
            break;
          }
          dcgmReturn_t result;

          result = _dcgmInit();
          if (result != DCGM_ST_OK)
            {
              DEBUG(stderr, "    -> Stopping GPU Tracking => Cannot initialize DCGM: %s\n\n", errorString(result));
              xalt_gpu_tracking = 0;
              dcgm_handle       = 0;
              break;
            }

          DCGMFUNC2(_dcgmStartEmbedded, DCGM_OPERATION_MODE_MANUAL, &dcgm_handle, &result);

          if (result != DCGM_ST_OK)
            {
              DEBUG(stderr, "    -> Stopping GPU Tracking => Cannot start DCGM: %s\n\n", errorString(result));
              xalt_gpu_tracking = 0;
              dcgm_handle       = 0;
              break;
            }

          if ( ! have_uuid )
            {
              int status = build_uuid(&uuid_str[0]);
              have_uuid = status == EXIT_SUCCESS;
            }

          result = _dcgmJobStartStats(dcgm_handle, (dcgmGpuGrp_t)DCGM_GROUP_ALL_GPUS, uuid_str);
          if (result != DCGM_ST_OK)
            {
              DEBUG(stderr, "    -> Stopping GPU Tracking => Cannot start DCGM job stats: %s\n\n", errorString(result));
              xalt_gpu_tracking = 0;
              dcgm_handle       = 0;
              break;
            }

          result = _dcgmWatchJobFields(dcgm_handle, (dcgmGpuGrp_t)DCGM_GROUP_ALL_GPUS, 1000, 1e9, 0);
          if (result != DCGM_ST_OK)
            {
              DEBUG(stderr,   "    -> Stopping GPU Tracking => Cannot start DCGM job watch: %s\n\n", errorString(result));
              if (result == DCGM_ST_REQUIRES_ROOT)
                DEBUG(stderr, "    -> May need to enable accounting mode: sudo nvidia-smi -am 1\n");
              xalt_gpu_tracking = 0;
              dcgm_handle       = 0;
              break;
            }

          result = _dcgmUpdateAllFields(dcgm_handle, 1);
          if (result != DCGM_ST_OK)
            {
              DEBUG(stderr, "    -> Stopping GPU Tracking => Cannot update DCGM job fields: %s\n\n", errorString(result));
              xalt_gpu_tracking = 0;
              dcgm_handle       = 0;
              break;
            }
#endif
        }
    }
  while(0);
  xalt_timer.gpu_setup = epoch() - t_gpu;
#endif

  if (xalt_gpu_tracking == 0)
    DEBUG(stderr, "  No GPU tracking\n");

  start_time = t0;
  frac_time  = start_time - (long) (start_time);

  /**********************************************************
   * Save LD_PRELOAD and clear it before running
   * run_submission().
   *********************************************************/

  p = getenv("LD_PRELOAD");
  if (p)
    ld_preload_strp = strdup(p);

  unsetenv("LD_PRELOAD");

  const char * blank = " ";

  /* Push XALT_RUN_UUID, XALT_DATE_TIME into the environment so that things like
   * R and python can know what job and what start time of the this run is.
   */

  if ( xalt_kind == BIT_PKGS )
    {
      if ( ! have_uuid )
        {
          int status = build_uuid(&uuid_str[0]);
          have_uuid = status == EXIT_SUCCESS;
        }
      setenv("XALT_RUN_UUID",uuid_str,1);
      DEBUG(stderr,"    -> Setting XALT_RUN_UUID: %s\n",uuid_str);
    }

  time_t my_time = (time_t) start_time;

  strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&my_time));
  sprintf(fullDateStr,"%s_%d",dateStr, (int) (frac_time*10000.0));

  char* my_xalt_dir = xalt_dir(NULL);
  setenv("XALT_DATE_TIME",fullDateStr,1);
  setenv("XALT_DIR",my_xalt_dir,1);
  my_free(my_xalt_dir, strlen(my_xalt_dir));

  ppid = getppid();

  // This routine returns either "FALSE" for nothing found or the watermark.
  bool have_watermark = xalt_vendor_note(&watermark, xalt_tracing);
  DEBUG(stderr,"    -> Found watermark via vendor note: %s\n", have_watermark ? "true" : "false");

  // If MPI program and no vendor watermark then try extracting the watermark
  // with objdump via extractXALTRecord(...)
  if (num_tasks > 1 && ! have_watermark )
    {
      have_watermark = extractXALTRecordString(exec_path, &watermark);
      DEBUG(stderr,"    -> Found watermark via objdump: %s\n", have_watermark ? "true" : "false");
    }

  if (! have_watermark) 
    watermark = strdup("FALSE");

  /*
   * XALT is only recording the end record for scalar executables and
   * not the start record.
   */

  v = getenv("XALT_SAMPLING");
  if (!v)
    {
      v = getenv("XALT_SCALAR_SAMPLING");
      if (!v)
        v = getenv("XALT_SCALAR_AND_SPSR_SAMPLING");
    }

  if (v && strcmp(v,"yes") == 0)
    {
      xalt_sampling     = 1;
      unsigned int a    = (unsigned int) clock();
      unsigned int b    = (unsigned int) time(NULL);
      unsigned int c    = (unsigned int) getpid();
      unsigned int seed = mix(a,b,c);

      srand(seed);
      my_rand       = (double) rand()/(double) RAND_MAX;
    }

  v = getenv("XALT_TESTING_RUNTIME");
  if (v)
    testing_runtime = strtod(v,NULL);

  always_record = xalt_mpi_always_record;
  v = getenv("XALT_MPI_ALWAYS_RECORD");
  if (v)
    always_record = strtol(v,(char **) NULL, 10);

  xalt_timer.init = epoch() - t0;
  // Create a start record for any MPI executions with an acceptable number of tasks.
  // or a PKG type
  if (num_tasks >= always_record )
    {
      if ( ! have_uuid )
        {
          int status = build_uuid(&uuid_str[0]);
          have_uuid = status == EXIT_SUCCESS;
        }

      if (have_uuid)
        {
          DEBUG(stderr, "    -> MPI_SIZE: %d >= MPI_ALWAYS_RECORD: %d => recording start record!\n",
                num_tasks, (int) always_record);
          if (xalt_tracing || xalt_run_tracing)
            fprintf(stderr, "  Recording state at beginning of %s user program:\n    %s\n",
                    xalt_run_short_descriptA[run_mask], exec_path);

          run_submission(&xalt_timer, orig_pid, ppid, start_time, end_time, probability, exec_path, num_tasks, num_gpus,
                         xalt_run_short_descriptA[xalt_kind], uuid_str, watermark, usr_cmdline, xalt_tracing,
                         always_record, stderr);
          DEBUG(stderr,"    -> uuid: %s\n", uuid_str);
        }
      else
        {
          DEBUG(stderr,"    -> Unable to produce Run UUID -> exiting\n}\n\n");
          reject_flag = XALT_NO_UUID;
          return;
        }


    }
  else
    {
      DEBUG(stderr,"    -> MPI_SIZE: %d < MPI_ALWAYS_RECORD: %d, XALT is build to %s, Current %s -> Not producing a start record\n",
             num_tasks, (int) always_record, xalt_build_descriptA[build_mask], xalt_run_descriptA[run_mask]);
    }

  /**********************************************************
   * Restore LD_PRELOAD after running xalt_run_submission.
   * This way the application and child apps will have
   * LD_PRELOAD set. (I'm looking at you mpiexec.hydra!)
   *********************************************************/

  if (ld_preload_strp)
    {
      setenv("LD_PRELOAD", ld_preload_strp, 1);
      memset(ld_preload_strp, 0, strlen(ld_preload_strp));
      free(ld_preload_strp);
    }

  /************************************************************
   * Register a signal handler wrapper_for_myfini for all the
   * important signals. This way a program terminated by
   * SIGFPE, SIGTERM, etc will produce an end record.
   *********************************************************/
  v = getenv("XALT_SIGNAL_HANDLER");
  if (!v)
    v = XALT_SIGNAL_HANDLER;
  if (strcasecmp(v,"yes") == 0)
    {
      DEBUG(stderr, "    -> Setting up signals\n");
      int signalA[] = {SIGHUP, SIGQUIT, SIGILL,  SIGTRAP, SIGABRT, SIGBUS,
                       SIGFPE, SIGTERM, SIGXCPU, SIGUSR1, SIGUSR2, SIGALRM};
      int signalSz  = N_ELEMENTS(signalA);
      struct sigaction action;
      struct sigaction old;
      memset(&action, 0, sizeof(struct sigaction));
      action.sa_handler = wrapper_for_myfini;
      for (i = 0; i < signalSz; ++i)
        {
          int signum = signalA[i];

          sigaction(signum, NULL, &old);

          if (old.sa_handler == NULL)
            sigaction(signum, &action, NULL);
        }
    }
  else
    DEBUG(stderr, "    -> Signals capturing disabled\n");
    
  v = getenv("XALT_DUMP_ENV");
  if (v && strcmp(v, "yes") == 0)
    {
      int i;
      for (i = 0; environ[i] != NULL; ++i)
        fprintf(stderr,"%s\n",environ[i]);
    }
  DEBUG(stderr, "    -> Leaving myinit\n}\n\n");
}
void wrapper_for_myfini(int signum)
{
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  sigemptyset( &action.sa_mask);
  action.sa_handler = SIG_DFL;
  sigaction(signum, &action, NULL);
  signal_hdlr_called = signum;
  myfini();
  raise(signum);
}

static void close_out(FILE* fp, int xalt_err)
{
  if (! fp)
    return;

  fflush(fp);
  if (xalt_err)
    {
      fclose(fp);
      close(errfd);
      close(STDERR_FILENO);
    }
}

void myfini()
{
  FILE * my_stderr = NULL;
  char * cmdline;
  char * v;
  double run_time;
  int    xalt_err = xalt_tracing || xalt_run_tracing;

  double t0 = epoch();
  if (xalt_err)
    {
      fflush(stderr);
      close(STDERR_FILENO);
      dup2(errfd, STDERR_FILENO);
      my_stderr = fdopen(errfd,"w");
      if ( !my_stderr )
        my_stderr = stderr;
    }


  set_end_record();  /* Mark my_free() to not free since we are on the way out */

  DEBUG(my_stderr,"\nmyfini(%ld/%d,%s,%s){\n", my_rank, num_tasks, STR(STATE), exec_path);
  if (orig_pid != getpid())
    {
      DEBUG(my_stderr,"    -> exiting because myfini() has been called more than once via forking\n}\n\n");
      close_out(my_stderr, xalt_err);
      return;
    }
    
  if (getenv("__XALT_FINAL_STATE__"))
    {
      DEBUG(my_stderr,"    -> exiting because myfini() has been called more than once\n}\n\n");
      close_out(my_stderr, xalt_err);
      return;
    }

  if (signal_hdlr_called)
    DEBUG(my_stderr,"    -> myfini() called via signal handler with signum: %d\n", signal_hdlr_called);

  /* Stop tracking if my mpi rank is not zero or the path was rejected. */
  if (reject_flag != XALT_SUCCESS)
    {
      if (xalt_kind == BIT_PKGS)
        remove_xalt_tmpdir(&uuid_str[0]);

      DEBUG(my_stderr,"    -> exiting because reject is set to: %s for program: %s\n}\n\n",
             xalt_reasonA[reject_flag], exec_path);
      close_out(my_stderr, xalt_err);
      return;
    }

  end_time = epoch();
  unsetenv("LD_PRELOAD");
  setenv("__XALT_FINAL_STATE__",    "1",1);

  // Sample all scalar executions and all MPI executions less than **always_record**
  if (num_tasks < always_record)
    {
      if (xalt_sampling)
        {
          double run_time;
          if (testing_runtime > -0.1)
            {
              run_time = testing_runtime;
              end_time = start_time + run_time;
            }
          else
            run_time  = end_time - start_time;
          probability = prgm_sample_probability(num_tasks, run_time);
          
          if (my_rand >= probability)
            {
              DEBUG(my_stderr, "    -> exiting because sampling. "
                     "run_time: %g, (my_rand: %g > prob: %g) for program: %s\n}\n\n",
                     run_time, my_rand, probability, exec_path);
              if (xalt_err && my_stderr)
                {
                  fclose(my_stderr);
                  close(errfd);
                  close(STDERR_FILENO);
                }
              return;
            }
          else
            DEBUG(my_stderr, "    -> Sampling program run_time: %g: (my_rand: %g <= prob: %g) for program: %s\n",
                   run_time, my_rand, probability, exec_path);
        }
      else
        DEBUG(my_stderr, "    -> XALT_SAMPLING = \"no\" All programs tracked!\n");
    }

#if USE_DCGM || USE_NVML
  /* This code will only ever be active in 64 bit mode and not 32 bit mode */
  if (xalt_gpu_tracking)
    {
      double t_gpu = epoch();
#ifdef USE_NVML
      nvmlReturn_t result;
      unsigned int device_count = 0;

      /* Get the number of GPU devices in the system */
      result = _nvmlDeviceGetCount(&device_count);
      if (result == NVML_SUCCESS)
        {
          DEBUG(my_stderr, "  %d GPUs detected\n", device_count);

          /* Loop over GPU devices */
          unsigned int i = 0;
          for (i = 0 ; i < device_count ; i++)
            {
              nvmlDevice_t device;
              nvmlEnableState_t mode;
              unsigned int pid_count = 0, max_pid_count = 0;
              unsigned int *pids = 0;
              unsigned int num_active_pids = 0;

              /* Get a device handle */
              result = _nvmlDeviceGetHandleByIndex(i, &device);
              if (result != NVML_SUCCESS)
                {
                  DEBUG(my_stderr, "  Unable to get device handle for GPU %d: %s\n", i, _nvmlErrorString(result));
                  continue;
                }

              /* Check whether accounting mode is enabled for this device */
              result = _nvmlDeviceGetAccountingMode(device, &mode);
              if (result != NVML_SUCCESS)
                {
                  DEBUG(my_stderr, "  Unable to get accounting mode for GPU %d: %s\n", i, _nvmlErrorString(result));
                  continue;
                }
              if (mode != NVML_FEATURE_ENABLED)
                {
                  DEBUG(my_stderr, "  Accounting mode is not enabled for GPU %d. Enable accounting mode: sudo nvidia-smi -i %d -am 1\n", i, i);
                  continue;
                }

              /* Get the size of the accounting buffer */
              result = _nvmlDeviceGetAccountingBufferSize(device, &max_pid_count);
              if (result != NVML_SUCCESS)
                {
                  DEBUG(my_stderr, "  Unable to get the accounting buffer size for GPU %d: %s\n", i, _nvmlErrorString(result));
                  continue;
                }

              /* Allocate space for the accounting data */
              pids = (unsigned int *)XMALLOC(sizeof(unsigned int)*max_pid_count);
              memset(pids, 0, sizeof(unsigned int)*max_pid_count);

              /* Get PID accounting data */
              pid_count = max_pid_count;
              result = _nvmlDeviceGetAccountingPids(device, &pid_count, pids);
              if (result != NVML_SUCCESS)
                {
                  DEBUG(my_stderr, "  Unable to get accounting data for GPU %d: %s\n", i, _nvmlErrorString(result));
                  my_free(pids, sizeof(unsigned int)*max_pid_count);
                  continue;
                }

              /* Loop over the PID accounting data looking for PIDs that
               * started after the program. Count those as belonging to
               * this program execution. */
              unsigned int j = 0;
              for (j = 0 ; j < pid_count ; j++)
                {
                  nvmlAccountingStats_t stats;

                  result = _nvmlDeviceGetAccountingStats(device, pids[j], &stats);
                  if (result == NVML_SUCCESS)
                    {
                      /* If the GPU PID start time is later than the
                         program start time, consider the GPU PID to
                         belong to the program.  This would not
                         necessarily be true if the system is
                         shared by unrelated programs. */
                      if (stats.startTime >= __time)
                        {
                          /* There is a mystery PID that appears for all
                             GPUs.  The PID is constant and is assumed
                             to be a CUDA cleanup process.  That PID
                             has a maxMemoryUsage of 0 while a real
                             PID will always have a non-zero value, so
                             use that to exclude it. */
                          if (stats.maxMemoryUsage > 0)
                            {
                              num_active_pids++;
                              DEBUG(my_stderr, "  PID %d startTime=%llu time=%llu isRunning=%d\n", pids[j], stats.startTime, stats.time, stats.isRunning);
                            }
                        }
                      /* Note that the GPU compute process has not
                         terminated yet.  Therefore stats.isRunning ==
                         1, and stats.time == 0, so it is not possible
                         to get the amount of time the GPU device was
                         actually in use. */
                    }
                }
              my_free(pids, sizeof(unsigned int)*max_pid_count);

              DEBUG(my_stderr, "  GPU %d: num compute pids %d\n", i, num_active_pids);

              if (num_active_pids > 0) {
                num_gpus++;
              }
            }

          /* Cleanup */
          result = _nvmlShutdown();
          if (result != NVML_SUCCESS)
            {
              DEBUG(my_stderr, "  Error shutting down NVML: %s\n", _nvmlErrorString(result));
            }
          dlclose(nvml_dl_handle);
        }
#elif USE_DCGM
      if (dcgm_handle != 0)
        {
          /* Collect DCGM job stats */
          dcgmReturn_t result;
          dcgmJobInfo_t job_info;

          DEBUG(my_stderr, "  GPU tracing\n");

          _dcgmUpdateAllFields(dcgm_handle, 1);
          _dcgmJobStopStats(dcgm_handle, uuid_str);

          job_info.version = dcgmJobInfo_version;
          result = _dcgmJobGetStats(dcgm_handle, uuid_str, &job_info);
          if (result == DCGM_ST_OK)
            {
              int i = 0;
              DEBUG(my_stderr, "  %d GPUs detected\n", job_info.numGpus);
              for (i = 0 ; i < job_info.numGpus ; i++)
                {
                  DEBUG(my_stderr, "  GPU %d: num compute pids %d\n", i, job_info.gpus[i].numComputePids);
                  if (job_info.gpus[i].numComputePids > 0)
                    num_gpus++;
                }
              DEBUG(my_stderr, "  %d of %d GPUs were used\n", num_gpus, job_info.numGpus);
            }

          _dcgmJobRemove(dcgm_handle, uuid_str);
          _dcgmStopEmbedded(dcgm_handle);
          _dcgmShutdown();
        }
#endif
      xalt_timer.gpu_setup += epoch() - t_gpu;
    }
#endif

  if (! have_uuid)
    {
      int status = build_uuid(&uuid_str[0]);
      have_uuid  = status == EXIT_SUCCESS;
    }

  if (! have_uuid )
    {
      DEBUG(my_stderr,"    -> Unable to build Run UUID -> exiting\n}\n\n");
      close_out(my_stderr, xalt_err);
      return;
    }

  if (xalt_tracing || xalt_run_tracing )
    {
      const char* location_getentropy = have_libc_getentropy_func() ? 
        "Using libc getentropy" :
        "Using XALT's getentropy";
    
      DEBUG(my_stderr,"    Run UUID: %s, %s UUID V7: %s\n", &uuid_str[0], location_getentropy,
            uuid_str[14] == '7' ? "yes" : "no");
      DEBUG(my_stderr,"    Recording State at end of %s user program\n",
            xalt_run_short_descriptA[run_mask]);
    }
  
  if (my_stderr)
    fflush(my_stderr);
  xalt_timer.fini = epoch() - t0;
  run_submission(&xalt_timer, orig_pid, ppid, start_time, end_time, probability, exec_path, num_tasks,
                 num_gpus, xalt_run_short_descriptA[xalt_kind], uuid_str, watermark,
                 usr_cmdline, xalt_tracing, always_record, my_stderr);
  DEBUG(my_stderr,"    -> leaving myfini\n}\n\n");
  build_uuid_cleanup();
  if (my_stderr)
    fflush(my_stderr);
  my_free(watermark,strlen(watermark));
  my_free(usr_cmdline,strlen(usr_cmdline));

  if (xalt_err && my_stderr)
    {
      fclose(my_stderr);
      close(errfd);
      close(STDERR_FILENO);
    }
}

#ifdef USE_NVML
static int load_nvml()
{
  /* Open the NVML library.  Let the dynamic loader find it (do not
     specify a path). then look in the $(LIB64) directory*/
  nvml_dl_handle = dlopen("libnvidia-ml.so", RTLD_LAZY);
  if ( ! nvml_dl_handle)
    {
      nvml_dl_handle = dlopen("libnvidia-ml.so.1", RTLD_LAZY);
      if ( ! nvml_dl_handle)
        {
          char* fn = xalt_dir("lib64/libnvidia-ml.so");
          nvml_dl_handle = dlopen(fn, RTLD_LAZY);
          if (fn)
            my_free(fn, strlen(fn));
          if ( ! nvml_dl_handle)
            {
              DEBUG(stderr, "    -> Unable to open libnvidia-ml.so or libnvidia-ml.so.1: %s\n\n",
                     dlerror());
              return 0;
            }
        }
    }

  /* Load symbols */
  *(void**)(&_nvmlDeviceGetAccountingBufferSize) =
    dlsym(nvml_dl_handle, "nvmlDeviceGetAccountingBufferSize");
  *(void**)(&_nvmlDeviceGetAccountingMode) =
    dlsym(nvml_dl_handle, "nvmlDeviceGetAccountingMode");
  *(void**)(&_nvmlDeviceGetAccountingPids) =
    dlsym(nvml_dl_handle, "nvmlDeviceGetAccountingPids");
  *(void**)(&_nvmlDeviceGetAccountingStats) =
    dlsym(nvml_dl_handle, "nvmlDeviceGetAccountingStats");
  *(void**)(&_nvmlDeviceGetCount) = dlsym(nvml_dl_handle, "nvmlDeviceGetCount");
  *(void**)(&_nvmlDeviceGetHandleByIndex) =
    dlsym(nvml_dl_handle, "nvmlDeviceGetHandleByIndex");
  *(void**)(&_nvmlErrorString) = dlsym(nvml_dl_handle, "nvmlErrorString");
  *(void**)(&_nvmlInit) = dlsym(nvml_dl_handle, "nvmlInit");
  *(void**)(&_nvmlShutdown) = dlsym(nvml_dl_handle, "nvmlShutdown");
  if (xalt_tracing)
    {
      struct link_map * map;
      int result = dlinfo(nvml_dl_handle, RTLD_DI_LINKMAP, &map);
      char* name = realpath(map->l_name, NULL);
      DEBUG(stderr, "    -> Successfully dynamically linked with nvidia-ml library: %s\n", name);
    }

  return 1;
}
#endif

#ifdef USE_DCGM
static int load_dcgm()
{
  char* fn       = xalt_dir("lib64/libdcgm.so");
  dcgm_dl_handle = dlopen(fn, RTLD_LAZY);
  if (fn)
    my_free(fn, strlen(fn));
  if ( ! dcgm_dl_handle)
    {
      fn             = xalt_dir("lib64/libdcgm.so.1");
      dcgm_dl_handle = dlopen(fn, RTLD_LAZY);
      if (fn)
        my_free(fn, strlen(fn));
      if ( ! dcgm_dl_handle)
        {
          DEBUG(stderr, "    -> Unable to open libdcgm.so or libdcgm.so.1: %s\n\n",
                 dlerror());
          return 0;
        }
    }

  /* Load symbols */
  *(void**)(&_dcgmInit)            = dlsym(dcgm_dl_handle, "dcgmInit");
  *(void**)(&_dcgmShutdown)        = dlsym(dcgm_dl_handle, "dcgmShutdown");
  *(void**)(&_dcgmStartEmbedded)   = dlsym(dcgm_dl_handle, "dcgmStartEmbedded");
  *(void**)(&_dcgmStopEmbedded)    = dlsym(dcgm_dl_handle, "dcgmStopEmbedded");
  *(void**)(&_dcgmJobStartStats)   = dlsym(dcgm_dl_handle, "dcgmJobStartStats");
  *(void**)(&_dcgmJobGetStats)     = dlsym(dcgm_dl_handle, "dcgmJobGetStats");
  *(void**)(&_dcgmJobRemove)       = dlsym(dcgm_dl_handle, "dcgmJobRemove");
  *(void**)(&_dcgmJobStopStats)    = dlsym(dcgm_dl_handle, "dcgmJobStopStats");
  *(void**)(&_dcgmUpdateAllFields) = dlsym(dcgm_dl_handle, "dcgmUpdateAllFields");
  *(void**)(&_dcgmWatchJobFields)  = dlsym(dcgm_dl_handle, "dcgmWatchJobFields");
  if (xalt_tracing)
    {
      struct link_map * map;
      int result = dlinfo(dcgm_dl_handle, RTLD_DI_LINKMAP, &map);
      char* name = realpath(map->l_name, NULL);
      DEBUG(stderr, "    -> Successfully dynamically linked with dcgm library: %s\n", name);
    }
  return 1;
}
#endif



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
static void get_abspath(char * path, int sz)
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

static  double prgm_sample_probability(int ntasks, double runtime)
{
  double prob = 1.0;
  int sz;
  int i;

  /* Pointer to an array of structs
     See https://stackoverflow.com/questions/16201607/c-pointer-to-array-of-structs
     for a discussion of how to do this.
   */


  interval_t (*p_interval)[];
  if (ntasks > 1)
    {
      sz         = mpi_rangeSz;
      p_interval = &mpi_rangeA;
    }
  else
    {
      sz         = scalar_rangeSz;
      p_interval = &scalar_rangeA;
    }

  for (i = 0; i < sz-1; ++i)
    {
      if (runtime < (*p_interval)[i+1].left)
        {
          prob = (*p_interval)[i].prob;
          break;
        }
    }
  return prob;
}





#ifndef XALT_MEMORY_TEST
#ifdef __MACH__
  __attribute__((section("__DATA,__mod_init_func"), used, aligned(sizeof(void*)))) __typeof__(myinit) *__init = myinit;
  __attribute__((section("__DATA,__mod_term_func"), used, aligned(sizeof(void*)))) __typeof__(myfini) *__fini = myfini;
#else
  __attribute__((section(".init_array"))) __typeof__(myinit) *__init = myinit;
  __attribute__((section(".fini_array"))) __typeof__(myfini) *__fini = myfini;
#endif
#endif
