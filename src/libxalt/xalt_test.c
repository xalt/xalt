#include <stdio.h>
#include <string.h>
#include "run_submission.h"
#include "xalt_obfuscate.h"
#include "epoch.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  double      t0	   = epoch();
  double      start_time   = t0 - 1234.567;
  double      end_time	   = t0;
  pid_t       pid	   = getpid();
  pid_t       ppid	   = getppid();
  double      prob	   = 1.0;
  const char* uuid	   = "b3c94e35-d779-438d-98a5-d19b39131ae8";
  const char* xalt_kind	   = "PKGS";
  const char* usr_cmdline  = "[\"hello.mpi\", \"--seq\", \"03\"]";
  char*       exec_name	   = argv[0];
  int         num_tasks	   = 1;
  int         xalt_tracing = 0;
  int         num_gpus	   = 0;
  const char* watermark	   = "XALT_Link_Info........<XALT_Version>%%2.8.8%%.<Build_host>%%jedrik"\
    "%%.<Build_Syshost>%%jedrik%%.<Build_compiler>%%gcc%%.<Build_compilerPath>%%/usr/bin/x86_64-linux-gnu-gcc-9%%."\
    "<Build_OS>%%Linux_%_%_5.4.0-40-generic%%.<Build_User>%%mclay%%.<Build_UUID>%%c4c04f8a-bd7f-4bee-be65-1b42ba6b25b9%%."\
    "<Build_Year>%%2020%%.<Build_date>%%Wed_%_%_Jul_%_%_15_%_%_22:24:08_%_%_2020%%.<Build_LOADEDMODULES>"\
    "%%unix/unix:local/local:mkl/mkl:noweb/2.11b:StdEnv:ddt/ddt:fdepend/fdepend:settarg:gcc/9.3.0:mpich/3.3.2%%."\
    "<Build_LMFILES>%%/opt/apps/modulefiles/Core/unix/unix.lua:/opt/apps/modulefiles/Core/local/local.lua:/opt/apps/modulefiles/Core/mkl/mkl.lua:/opt/apps/modulefiles/Core/noweb/2.11b.lua:/opt/apps/modulefiles/Core/StdEnv.lua:/opt/apps/modulefiles/Core/ddt/ddt.lua:/opt/apps/modulefiles/Core/fdepend/fdepend.lua:/opt/apps/lmod/lmod/modulefiles/Core/settarg.lua:/opt/apps/modulefiles/Core/gcc/9.3.0.lua:/opt/apps/modulefiles/Compiler/gcc/9/mpich/3.3.2.lua%%."\
    "<Build_CWD>%%/home/mclay/w/xalt/rt/end2end/t1/x86_64_06_avx2_dbg_gcc-9.3.0_mpich-3.3.2-2020_07_14_15_51_54-Linux-x86_64-end2end%%.<Build_Epoch>%%1594869848.4215%%........XALT_Link_Info_End.";
  run_submission(t0, pid, ppid, start_time, 0.0, prob, exec_name, num_tasks,
  		 num_gpus, xalt_kind, uuid, watermark, usr_cmdline, xalt_tracing, stderr);
  
  fprintf(stderr,"round two!\n");
  run_submission(t0, pid, ppid, start_time, end_time, prob, exec_name, num_tasks,
  		 num_gpus, xalt_kind, uuid, watermark, usr_cmdline, xalt_tracing, stderr);

  fprintf(stderr,"done!\n");
  return 0;
}
