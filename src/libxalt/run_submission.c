#include <time.h>
#include <string.h>

#include "xalt_config.h"
#include "xalt_syshost.h"
#include "buildEnvT.h"
#include "buildJson.h"
#include "buildXALTRecord.h"
#include "compute_sha1.h"
#include "insert.h"
#include "processTree.h"
#include "parseProcMaps.h"
#include "translate.h"
#include "parseProcMaps.h"
#include "buildJson.h"
#include "run_submission.h"
#include "epoch.h"

static const char* blank0      = "";
static const char* comma       = ",";

void run_submission(double t0, pid_t pid, pid_t ppid, double start_time, double end_time, double probability,
		    char* exec_path, long my_size, int num_gpus, const char* xalt_kind, const char* uuid_str,
		    const char* watermark, const char* usr_cmdline, char* env[], FILE* my_stderr)
{
  char *      	 p_dbg        = getenv("XALT_TRACING");
  int         	 xalt_tracing = (p_dbg && ( strncmp(p_dbg,"yes",3) == 0 || strncmp(p_dbg,"run",3) == 0));
  bool        	 end_record   = (end_time > 0.0);
  const char* 	 suffix       = end_record ? ".zzz" : ".aaa";

  processTree_t* ptA      = NULL;
  S2S_t*      	 envT	  = NULL;
  S2S_t*      	 recordT  = NULL;
  S2S_t       	 userT	  = NULL;
  S2D_t       	 userDT	  = NULL;
  S2D_t       	 measureT = NULL;
  SET_t          libT     = NULL;
  double      	 t1;

  //************************************************************
  // Walk Process tree to find parent processes

  t1 = epoch();
  walkProcessTree(ppid, &ptA);
  insert_key_double(&measureT, "04_WalkProcTree_", epoch() - t1);
  DEBUG0(my_stderr,"    Built processTree table\n");
  
  //************************************************************
  // Walk env to build the full env table
  // (ignoring shell functions)
  t1 = epoch();
  buildEnvT(env, &envT);
  DEBUG0(my_stderr,"    Built envT\n");
  insert_key_double(&measureT, "03_BuildEnvT_____", epoch() - t1);

  //************************************************************
  // Build recordT from the watermark
  t1 = epoch();
  buildXALTRecordT(watermark, &recordT);
  insert_key_double(&measureT, "05_Build_RecordT_", epoch() - t1);
  DEBUG0(my_stderr,"    Extracted recordT from watermark\n");

  //*********************************************************************
  // Build userT, userDT

  t1 = epoch();
  S2S_t* e;
  if (recordT)
    {
      HASH_FIND_STR(recordT, "Build_Epoch", e);
      if (e)
	insert_key_double(&userDT, "Build_Epoch",  strtod(e->value, (char**) NULL));
    }
	  
  char* exec_pathQ = strdup(xalt_quotestring(exec_path));
  xalt_quotestring_free();

  double run_time = (end_record) ? end_time - start_time : 0.0;
  insert_key_double(&userDT, "start_time",  start_time);
  insert_key_double(&userDT, "end_time",    end_time);
  insert_key_double(&userDT, "run_time",    run_time);
  insert_key_double(&userDT, "probability", probability);
  insert_key_double(&userDT, "num_tasks",   num_tasks);
  insert_key_double(&userDT, "num_gpus",    num_gpus);

  insert_key_string(&userT,  "syshost",     xalt_syshost());
  insert_key_string(&userT,  "run_uuid",    uuid_str);
  insert_key_string(&userT,  "exec_path",   exec_pathQ);
  insert_key_string(&userT,  "exec_type",   "binary");
  free(exec_pathQ);
  buildUserT(&userT, &userDT);
  translate(envT, userT, &userDT);

  HASH_FIND_STR(userT, "scheduler", e);
  const char * scheduler = (e) ? e->value : "not known";
  insert_key_double(&measureT, "01_BuildUserT___", epoch() - t1);
  DEBUG1(my_stderr,"    Built userT, userDT, scheduler: %s\n", scheduler);

  //*********************************************************************
  // Filter envT 

  t1 = epoch();
  filterEnvT(env, &envT);
  DEBUG0(my_stderr,"    Filter envT\n");
  insert_key_double(&measureT, "03_BuildEnvT____", epoch() - t1);

  //*********************************************************************
  // Take sha1sum of the executable
  t1 = epoch();
  char sha1buf[41];
  compute_sha1(exec_path, &sha1buf);
  DEBUG1(my_stderr,"    Compute sha1 of exec: %s\n",exec_path);
  insert_key_double(&measureT, "02_Sha1_exec____", epoch() - t1);

    
  //*********************************************************************
  // Parse the /proc/$pid/map file for the shared libraries
  t1 = epoch();
  parseProcMaps(pid, &libT);
  DEBUG0(my_stderr,"    Parsed ProcMaps\n");
  insert_key_double(&measureT, "06_ParseProcMaps", epoch() - t1);

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;
  
  DEBUG1(my_stderr,"    Using XALT_TRANSMISSION_STYLE: %s\n",transmission);

  measureT["07____total_____"] = epoch() - t0;

  //*********************************************************************
  // So build the Json table string
  
  char*  jsonStr;
  Json_t json;
  const char* my_sep = blank0;
  json_init(Json_TABLE, &json);
  json_add_json_str( &json, my_sep, "cmdlineA",  usr_cmdline);   my_sep = comma;
  json_add_ptA(      &json, my_sep, "ptA",       ptA);
  json_add_S2S(      &json, my_sep, "envT",      envT);
  json_add_S2S(      &json, my_sep, "userT",     userT);
  json_add_S2D(      &json, my_sep, "userDT",    userDT);
  json_add_S2S(      &json, my_sep, "xaltLinkT", recordT);
  json_fini(         &json, &json_result_str);
  DEBUG0(stderr,"    Built json string\n");

  


}

