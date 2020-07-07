#include <time.h>
#include <string.h>

#include "xalt_config.h"
#include "buildEnvT.h"
#include "buildJson.h"
#include "buildXALTRecord.h"
#include "compute_sha1.h"
#include "insert.h"
#include "processTree.h"
#include "parseProcMaps.h"
#include "translate.h"
#include "run_submission.h"
#include "epoch.h"

void run_submission(pid_t pid, pid_t ppid, double start_time, double end_time, double probability,
		    char* exec_pathQ, long my_size, int num_gpus, int xalt_type,
                    const char* uuid_str, const char* watermark, const char* usr_cmdline,
		    char* env[])
{
  char *      	 p_dbg        = getenv("XALT_TRACING");
  int         	 xalt_tracing = (p_dbg && ( strncmp(p_dbg,"yes",3) == 0 || strncmp(p_dbg,"run",3) == 0));
  bool        	 end_record   = (end_time > 0.0);
  const char* 	 suffix       = end_record ? ".zzz" : ".aaa";

  processTree_t* ptA;
  S2S_t*      	 envT;
  S2S_t*      	 recordT;
  S2S_t       	 userT;
  S2D_t       	 userDT;
  S2D_t       	 measureT;
  double      	 t0, t1;

  //************************************************************
  // Walk Process tree to find parent processes

  t0 = epoch();
  walkProcessTree(ppid, &ptA);
  insert_key_double(&measureT, "04_WalkProcTree_", epoch() - t0);
  DEBUG0(stderr,"    Built processTree table\n");
  
  //************************************************************
  // Walk env to build the full env table
  // (ignoring shell functions)
  t1 = epoch();
  buildEnvT(env, &envT);
  DEBUG0(stderr,"    Built envT\n");
  insert_key_double(&measureT, "03_BuildEnvT_____", epoch() - t1);

  //************************************************************
  // Build recordT from the watermark
  t1 = epoch();
  buildXALTRecordT(watermark, &recordT);
  insert_key_double(&measureT, "05_Build_RecordT_", epoch() - t1);
  DEBUG0(stderr,"    Extracted recordT from watermark\n");

  //*********************************************************************
  // Build userT, userDT

  // Questions: syshost?


  t1 = epoch();
  buildUserT(



}

