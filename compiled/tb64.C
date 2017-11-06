#include "base64.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>
#include "zstring.h"
#include <string>

int main(int argc, char* argv[])
{
  const char* jsonStr = "{\"cmdlineA\":[\"./hello.x\"],\"ptA\":[{\"cmd_name\":\"hydra_pmi_proxy\",\"cmd_path\":\"/opt/apps/gcc-6_3/mpich/3.2/bin/hydra_pmi_proxy\",\"pid\":29252,\"cmdlineA\":[\"/opt/apps/gcc-6_3/mpich/3.2/bin/hydra_pmi_proxy\",\"--control-port\",\"rios:33619\",\"--rmk\",\"user\",\"--launcher\",\"ssh\",\"--demux\",\"poll\",\"--pgid\",\"0\",\"--retries\",\"10\",\"--usize\",\"-2\",\"--proxy-id\",\"0\"]},{\"cmd_name\":\"mpirun\",\"cmd_path\":\"/opt/apps/gcc-6_3/mpich/3.2/bin/mpiexec.hydra\",\"pid\":29251,\"cmdlineA\":[\"mpirun\",\"-n\",\"1\",\"./hello.x\"]},{\"cmd_name\":\"t1.script\",\"cmd_path\":\"/bin/bash\",\"pid\":27563,\"cmdlineA\":[\"/bin/bash\",\"./t1.script\"]},{\"cmd_name\":\"sh\",\"cmd_path\":\"/bin/dash\",\"pid\":27562,\"cmdlineA\":[\"sh\",\"-c\",\"./t1.script > t1.log 2>&1 < /dev/null\"]},{\"cmd_name\":\"python3\",\"cmd_path\":\"/usr/bin/python3.5\",\"pid\":27559,\"cmdlineA\":[\"python3\",\"/home/mclay/w/themis/lib/themis.py\",\"/home/mclay/w/themis\",\"/home/mclay/w/themis/bin/rtm\",\".\"]},{\"cmd_name\":\"themis_starter\",\"cmd_path\":\"/bin/bash\",\"pid\":27556,\"cmdlineA\":[\"/bin/bash\",\"/home/mclay/w/themis/bin/themis_starter\",\"/home/mclay/w/themis/bin/rtm\",\".\"]},{\"cmd_name\":\"rtm\",\"cmd_path\":\"/bin/dash\",\"pid\":27555,\"cmdlineA\":[\"/bin/sh\",\"/home/mclay/w/themis/bin/rtm\",\".\"]},{\"cmd_name\":\"t\",\"cmd_path\":\"/bin/dash\",\"pid\":27554,\"cmdlineA\":[\"/bin/sh\",\"/home/mclay/.up/bin/t\",\".\"]},{\"cmd_name\":\"zsh\",\"cmd_path\":\"/bin/zsh\",\"pid\":1849,\"cmdlineA\":[\"zsh\"]},{\"cmd_name\":\"xterm\",\"cmd_path\":\"/usr/bin/xterm\",\"pid\":1847,\"cmdlineA\":[\"/usr/bin/xterm\",\"-sb\",\"-vb\"]}],\"envT\":{\"MODULEPATH\":\"/opt/apps/modulefiles/MPI/gcc/6.3/mpich/3.2:/opt/apps/modulefiles/Compiler/gcc/6.3:/opt/apps/modulefiles/Linux:/opt/apps/modulefiles/Core:/opt/apps/lmod/lmod/modulefiles/Core\",\"_LMFILES_\":\"/opt/apps/modulefiles/Core/unix/unix.lua:/opt/apps/modulefiles/Core/local/local.lua:/opt/apps/modulefiles/Core/mkl/mkl.lua:/opt/apps/modulefiles/Core/noweb/2.11b.lua:/opt/apps/modulefiles/Core/StdEnv.lua:/opt/apps/modulefiles/Core/autotools/1.2.lua:/opt/apps/modulefiles/Core/ddt/ddt.lua:/opt/apps/modulefiles/Core/fdepend/fdepend.lua:/opt/apps/lmod/lmod/modulefiles/Core/settarg/7.7.13.lua:/opt/apps/modulefiles/Core/gcc/6.3.0.lua:/opt/apps/modulefiles/Compiler/gcc/6.3/mpich/3.2.lua\",\"PATH\":\"/home/mclay/w/xalt/rt/syslog/t1/x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2-2017_11_06_12_43_03-Linux-x86_64-syslog/XALT/bin:/home/mclay/w/xalt/rt/syslog/t1/x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2-2017_11_06_12_43_03-Linux-x86_64-syslog/XALT/sbin:/opt/apps/gcc-6_3/mpich/3.2/bin:/home/mclay/w/xalt/rt/syslog/t1/x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2-2017_11_06_12_43_03-Linux-x86_64-syslog/XALT/xalt/xalt/bin:/home/mclay/w/xalt/rt/syslog/t1/x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2-2017_11_06_12_43_03-Linux-x86_64-syslog/XALT/xalt/xalt/sbin:/home/mclay/w/xalt/rt/syslog/t1/x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2-2017_11_06_12_43_03-Linux-x86_64-syslog/XALT/bin:/home/mclay/w/xalt/rt/syslog/t1/x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2-2017_11_06_12_43_03-Linux-x86_64-syslog/XALT/sbin:/home/mclay/w/xalt/rt/syslog/bin:/home/mclay/w/dao/cfdtools/bin:/home/mclay/.up/tools:/home/mclay/w/usefulTools/bin:/home/mclay/bin/_x86_64:/home/mclay/bin:/home/mclay/.up/bin:OBJ/_x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2:/opt/apps/lmod/lmod/settarg:/opt/apps/fdepend/fdepend/bin:/opt/apps/ddt/ddt/bin:/opt/apps/autotools/1.2/bin:/home/mclay/.local/bin:/opt/apps/noweb/2.11b/bin:/opt/apps/icon/icon/bin:/usr/local/share/bin:/opt/local/bin:/opt/apps/luarocks/luarocks/bin:/opt/apps/lua/lua/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/home/mclay/w/hermes/bin:/home/mclay/w/themis/bin\",\"LOADEDMODULES\":\"unix/unix:local/local:mkl/mkl:noweb/2.11b:StdEnv:autotools/1.2:ddt/ddt:fdepend/fdepend:settarg/7.7.13:gcc/6.3.0:mpich/3.2\",\"LD_LIBRARY_PATH\":\"/opt/apps/gcc-6_3/mpich/3.2/lib\",\"PYTHONPATH\":\"/home/mclay/w/themis/lib\"},\"userT\":{\"account\":\"unknown\",\"submit_host\":\"unknown\",\"queue\":\"unknown\",\"run_uuid\":\"bf797973-0edc-46e8-9162-3484aaddb6da\",\"currentEpoch\":\"1509993797.939079\",\"syshost\":\"rios\",\"exec_type\":\"binary\",\"start_date\":\"Mon Nov  6 12:43:17 2017\",\"user\":\"mclay\",\"exec_path\":\"/home/mclay/w/xalt/rt/syslog/t1/x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2-2017_11_06_12_43_03-Linux-x86_64-syslog/hello.x\",\"cwd\":\"/home/mclay/w/xalt/rt/syslog/t1/x86_64_06_avx2_dbg_gcc-6.3.0_mpich-3.2-2017_11_06_12_43_03-Linux-x86_64-syslog\",\"execModify\":\"Mon Nov  6 12:43:17 2017\",\"job_id\":\"unknown\"},\"userDT\":{\"num_nodes\":1.000000,\"num_cores\":1.000000,\"exec_epoch\":1509993797.000000,\"currentEpoch\":1509993797.939079,\"start_time\":1509993797.921000,\"end_time\":0.000000,\"run_time\":0.000000,\"Build_Epoch\":1509993797.705900,\"num_threads\":1.000000,\"num_tasks\":1.000000},\"xaltLinkT\":{\"Build_LMFILES\":\"/opt/apps/modulefiles/Core/unix/unix.lua:/opt/apps/modulefiles/Core/local/local.lua:/opt/apps/modulefiles/Core/mkl/mkl.lua:/opt/apps/modulefiles/Core/noweb/2.11b.lua:/opt/apps/modulefiles/Core/StdEnv.lua:/opt/apps/modulefiles/Core/autotools/1.2.lua:/opt/apps/modulefiles/Core/ddt/ddt.lua:/opt/apps/modulefiles/Core/fdepend/fdepend.lua:/opt/apps/lmod/lmod/modulefiles/Core/settarg/7.7.13.lua:/opt/apps/modulefiles/Core/gcc/6.3.0.lua:/opt/apps/modulefiles/Compiler/gcc/6.3/mpich/3.2.lua\",\"Build_LOADEDMODULES\":\"unix/unix:local/local:mkl/mkl:noweb/2.11b:StdEnv:autotools/1.2:ddt/ddt:fdepend/fdepend:settarg/7.7.13:gcc/6.3.0:mpich/3.2\",\"Build_Epoch\":\"1509993797.7059\",\"Build_Syshost\":\"rios\",\"Build_User\":\"mclay\",\"Build_compiler\":\"gcc\",\"Build_OS\":\"Linux 4.10.0-38-generic\",\"Build_host\":\"rios\",\"XALT_Version\":\"1.8-devel\",\"Build_compilerPath\":\"/usr/bin/x86_64-linux-gnu-gcc-6\",\"Build_Year\":\"2017\",\"Build_UUID\":\"0a59abf0-e48a-4e26-b3e2-d869754d5037\",\"Build_date\":\"Mon Nov 06 12:43:17 2017\"},\"hash_id\":\"62b018fedfa30bd7a77e4c6b7856426fbce0eaf3\",\"libA\":[[\"/opt/apps/gcc-6_3/mpich/3.2/lib/libmpi.so.12\",\"0\"],[\"/lib/x86_64-linux-gnu/libc.so.6\",\"0\"],[\"/lib/x86_64-linux-gnu/libuuid.so.1\",\"0\"],[\"/lib/x86_64-linux-gnu/librt.so.1\",\"0\"],[\"/lib/x86_64-linux-gnu/libpthread.so.0\",\"0\"],[\"/lib/x86_64-linux-gnu/libgcc_s.so.1\",\"0\"]],\"XALT_measureT\":{\"07____total_____\":0.030386,\"02_Sha1_exec____\":0.000620,\"04_WalkProcTree_\":0.000699,\"06_ParseLDD_sha1\":0.000053,\"03_BuildEnvT____\":0.002094,\"05_ExtractXALTR_\":0.006174,\"06_ParseLDD_ldd_\":0.020556,\"01_BuildUserT___\":0.000169}}";
  int lenJson = strlen(jsonStr);
  int b64len;
  char* b64   = base64_encode(jsonStr, lenJson, &b64len);

  int lenJ2;
  unsigned char* jstr = base64_decode(b64, b64len, &lenJ2);

  free(b64);

  printf("%d, %d, %s\n",lenJson, lenJ2, jstr);

  int zslen;

  char * zs    = compress_string(jsonStr, &zslen);

  char * js = uncompress_string(zs, zslen);


  printf("%ld %%%s%%\n",strlen(js), js);

  free(js);

  b64 = base64_encode(zs, zslen, &b64len);

  zs = (char *) base64_decode(b64, b64len, &zslen);

  js = uncompress_string(zs,zslen);
  
  printf("%ld %%%s%%\n",strlen(js), js);

  free(js);
  return 0;
}
