#include <time.h>
#include <string.h>
#include <stdio.h>
#include "xalt_config.h"
#include "xalt_syshost.h"
#include "buildEnvT.h"
#include "buildJson.h"
#include "buildXALTRecordT.h"
#include "compute_sha1.h"
#include "insert.h"
#include "processTree.h"
#include "parseProcMaps.h"
#include "translate.h"
#include "parseProcMaps.h"
#include "buildJson.h"
#include "run_submission.h"
#include "epoch.h"

static const char* blank0 = "";
static const char* comma  = ",";
extern char **environ;

void run_submission(double t0, pid_t pid, pid_t ppid, double start_time, double end_time, double probability,
		    char* exec_path, long my_size, int num_gpus, const char* xalt_kind, const char* uuid_str,
		    const char* watermark, const char* usr_cmdline, FILE* my_stderr)
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

  DEBUG1(my_stderr,"\n  run_submission(%s) {\n",suffix);

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
  buildEnvT(environ, &envT);
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

  const char* syshost  = xalt_syshost();
  double      run_time = (end_record) ? end_time - start_time : 0.0;
  insert_key_double(&userDT, "start_time",  start_time);
  insert_key_double(&userDT, "end_time",    end_time);
  insert_key_double(&userDT, "run_time",    run_time);
  insert_key_double(&userDT, "probability", probability);
  insert_key_double(&userDT, "num_tasks",   num_tasks);
  insert_key_double(&userDT, "num_gpus",    num_gpus);

  insert_key_string(&userT,  "syshost",     syshost);
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
  json_add_SET(      &json, my_sep, "libA"       libT);
  json_add_ptA(      &json, my_sep, "ptA",       ptA);
  json_add_S2S(      &json, my_sep, "envT",      envT);
  json_add_S2S(      &json, my_sep, "userT",     userT);
  json_add_S2D(      &json, my_sep, "userDT",    userDT);
  json_add_S2S(      &json, my_sep, "xaltLinkT", recordT);
  json_fini(         &json, &jsonStr);
  DEBUG0(stderr,"    Built json string\n");

  char* resultFn  = NULL;
  char* resultDir = NULL;  

  char key[50];
  sprintf(&key[0], "%s%s", (end_record) ? "run_fini_" : "run_strt_", uuid_str);

  if (strcasecmp(transmission, "file") == 0 || strcasecmp(transmission, "file_separate_dirs") == 0)
    {
      build_resultDir(&resultDir, "run", transmission, uuid_str);
      build_resultFn( &resultFn,  "run", start_time, syshost, uuid_str, suffix);
    }

  transmit(transmission, jsonStr, "run", key, syshost, resultDir, resultFn);
  xalt_quotestring_free();
  if (resultFn)
    {
      free(resultFn);
      free(resultDir);
    }

  if (strcmp(xalt_kind,"PKGS") == 0)
    pkgRecordTransmit(uuid_str, syshost, transmission);

  DEBUG0(stderr,"  }\n\n");
  if (xalt_tracing)
    fflush(my_stderr);
}

void pkgRecordTransmit(const char* uuid_str, const char* syshost, const char* transmission)
{
  char * xalt_tmpdir = create_xalt_tmpdir_str(uuid_str);
  DIR*   dirp        = opendir(xalt_tmpdir);
  if (dirp == NULL)
    {
      free(xalt_tmpdir);
      return;
    }

  char* c_home = getenv("HOME");
  char* c_user = getenv("USER");
  if (c_home == NULL || c_user == NULL )
    return;

  int         ulen      = 12;
  char*       resultDir = NULL;
  
  if (strcasecmp(transmission, "file") == 0 || strcasecmp(transmission, "file_separate_dirs") == 0)
    build_resultDir(&resultDir, "pkg", transmission, uuid_str);

  UT_string *jsonStr, *fullName, *key;
  utstring_new(jsonStr);
  utstring_new(fullName);
  utstring_new(key);

  struct dirent* dp;
  while ( (dp = readdir(dirp)) != NULL)
    {
      if (fnmatch("pkg.*.json", dp->d_name, 0) == 0)
        {
          char*       buf     = NULL;
          size_t      sz      = 0;
	  utstring_clear(jsonStr);
	  utstring_clear(fullName);
	  utstring_clear(key);

	  utstring_printf(fullName,"%s/%s",xalt_tmpdir,dp->d_name);
          FILE* fp = fopen(utstring_body(fullName), "r");
          if (fp)
            {
              while( xalt_fgets_alloc(fp, &buf, &sz))
		utstring_bincpy(jsonStr, buf, strlen(buf));
              free(buf);
              sz = 0; buf = NULL;

              // build key from dp->d_name;
              //                                                                           0123456789 1234567
              //pkg.rios.2018_11_06_16_14_13_7992.user.d20188d7-bbbb-4b91-9f5c-80672045c270.3ee8e5affda9.json
              int my_len = strlen(dp->d_name);
	      utstring_printf(key,"pkg_%s_%.*s",uuid_str, ulen, &dp->d_name[my_len - 17]);
              // transmit jsonStr
              
              transmit(transmission, utstring_body(jsonStr), "pkg", utstring_body(key), syshost,
		       resultDir, dp->d_name);
              unlink(utstring_body(fullName));
            }
          fclose(fp);
        }
    }
  utstring_free(jsonStr);
  utstring_free(fullName);
  utstring_free(key);
  rmdir(xalt_tmpdir);
  free(resultDir);
  free(xalt_tmpdir);
}
