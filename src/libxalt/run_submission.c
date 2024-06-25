#include <time.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <fnmatch.h>
#include "xalt_config.h"
#include "xalt_syshost.h"
#include "buildEnvT.h"
#include "buildJson.h"
#include "buildXALTRecordT.h"
#include "crc.h"
#include "compute_sha1.h"
#include "insert.h"
#include "processTree.h"
#include "parseProcMaps.h"
#include "translate.h"
#include "parseProcMaps.h"
#include "xalt_quotestring.h"
#include "buildJson.h"
#include "run_submission.h"
#include "epoch.h"
#include "xalt_types.h"
#include "xalt_c_utils.h"
#include "xalt_tmpdir.h"
#include "xalt_fgets_alloc.h"
#include "buildUserT.h"
#include "transmit.h"

static const char *blank0 = "";
static const char *comma  = ",";
extern char        **environ;
static char        sha1buf[41];
static char        buff[40];
static bool        need_sha1 = true;

void run_submission(xalt_timer_t *xalt_timer, pid_t pid, pid_t ppid, double start_time, double end_time, double probability,
                    char* exec_path, int num_tasks, int num_gpus, const char* xalt_kind, const char* uuid_str,
                    const char* watermark, const char* usr_cmdline, int xalt_tracing, long mpi_always_record, FILE* my_stderr)
{
  bool           end_record   = (end_time > 0.0);
  const char*    suffix       = end_record ? ".zzz" : ".aaa";

  processTree_t* ptA      = NULL;
  S2S_t*         qaT      = NULL;
  S2S_t*         envT     = NULL;
  S2S_t*         recordT  = NULL;
  S2S_t*         userT    = NULL;
  S2D_t*         userDT   = NULL;
  S2D_t*         measureT = NULL;
  SET_t*         libT     = NULL;
  double         t0       = epoch();
  double         t1;

  DEBUG(my_stderr,"\n  run_submission(%s) {\n",suffix);

  //************************************************************
  // Walk Process tree to find parent processes

  t1 = epoch();
  walkProcessTree(ppid, &ptA);
  insert_key_double(&measureT, "04_WalkProcTree__", epoch() - t1);
  DEBUG(my_stderr,"    Built processTree table\n");
  
  //************************************************************
  // Walk env to build the filtered env table
  // and ignore shell functions
  t1 = epoch();
  buildEnvT(environ, &envT);
  DEBUG(my_stderr,"    Built envT\n");
  insert_key_double(&measureT, "03_BuildEnvT_____", epoch() - t1);

  //************************************************************
  // Build recordT from the watermark
  t1 = epoch();
  buildXALTRecordT(watermark, &recordT);
  insert_key_double(&measureT, "05_Build_RecordT_", epoch() - t1);

  //*********************************************************************
  // Build userT, userDT

  t1 = epoch();
  S2S_t* e;
  if (recordT)
    {
      char* compiler;
      char* xalt_version;

      HASH_FIND_STR(recordT, "Build_Epoch", e);
      if (e)
        insert_key_double(&userDT, "Build_Epoch",  strtod(utstring_body(e->value), (char**) NULL));
      
      if (xalt_tracing)
        {
          HASH_FIND_STR(recordT, "Build_compiler", e);
          if (e)
            compiler = strdup(utstring_body(e->value));
          else
            compiler = strdup("UNKNOWN");
          HASH_FIND_STR(recordT, "XALT_Version", e);
          if (e)
            xalt_version = strdup(utstring_body(e->value));
          else
            xalt_version = strdup("UNKNOWN");
          DEBUG(my_stderr,"    Extracted recordT from watermark. Compiler: %s, XALT_Version: %s\n",
                compiler,xalt_version);
          my_free(compiler,strlen(compiler));
          my_free(xalt_version,strlen(xalt_version));
        }
    }
  else
    DEBUG(my_stderr,"    No watermark found -> recordT is empty\n");
    
          
  char* exec_pathQ = strdup(xalt_quotestring(exec_path));
  xalt_quotestring_free();

  char*  syshost  = xalt_syshost();
  double run_time = (end_record) ? end_time - start_time : 0.0;
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
  my_free(exec_pathQ,strlen(exec_pathQ));
  buildUserT(&userT, &userDT);
  translate(&userT, &userDT);

  HASH_FIND_STR(userT, "scheduler", e);
  const char * scheduler = (e) ? utstring_body(e->value) : "not known";
  insert_key_double(&measureT, "01_BuildUserT____", epoch() - t1);
  DEBUG(my_stderr,"    Built userT, userDT, scheduler: %s\n", scheduler);

  //*********************************************************************
  // Take sha1sum of the executable
  if (need_sha1)
    {
      t1 = epoch();
      compute_sha1(exec_path, &sha1buf[0]);
      compute_sha1_cleanup();
      DEBUG(my_stderr,"    Compute sha1 (%s) of exec: %s\n",&sha1buf[0], exec_path);
      insert_key_double(&measureT, "02_Sha1_exec_____", epoch() - t1);
      need_sha1 = false;
    }
  else
    {
      insert_key_double(&measureT, "02_Sha1_exec_____", 0.0);
      DEBUG(my_stderr,"    Reuse   sha1 (%s) of exec: %s\n",&sha1buf[0], exec_path);
    }
      
  //*********************************************************************
  // Parse the /proc/$pid/map file for the shared libraries
  t1 = epoch();
  parseProcMaps(pid, &libT);
  DEBUG(my_stderr,"    Parsed ProcMaps\n");
  insert_key_double(&measureT, "06_ParseProcMaps_", epoch() - t1);

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;
  
  DEBUG(my_stderr,"    Using XALT_TRANSMISSION_STYLE: %s\n",transmission);

  insert_key_double(&measureT, "07_GPU_Setup_____", xalt_timer->gpu_setup);
  insert_key_double(&measureT, "08____total______", epoch() - t0 + xalt_timer->init + xalt_timer->fini);

  char* resultFn  = NULL;
  char* resultDir = NULL;  

  //*********************************************************************
  // Record QA data in json string.
  insert_key_string(&qaT,"XALT_GIT_VERSION",XALT_GIT_VERSION);

  sprintf(&buff[0],"%ld",mpi_always_record);
  insert_key_string(&qaT,"XALT_MPI_ALWAYS_RECORD", buff);
  const char* xalt_sampling = getenv("XALT_SAMPLING");

  const char* xalt_mpi_tracking = getenv("XALT_MPI_TRACKING");
  if (xalt_mpi_tracking == NULL)
    xalt_mpi_tracking = XALT_MPI_TRACKING;
  insert_key_string(&qaT,"XALT_MPI_TRACKING", xalt_mpi_tracking);

  const char* xalt_scalar_tracking = getenv("XALT_SCALAR_TRACKING");
  if (xalt_scalar_tracking == NULL)
    xalt_scalar_tracking = XALT_SCALAR_TRACKING;
  insert_key_string(&qaT,"XALT_SCALAR_TRACKING", xalt_scalar_tracking);

  if (!xalt_sampling)
    {
      xalt_sampling = getenv("XALT_SCALAR_SAMPLING");
      if (!xalt_sampling)
        xalt_sampling = getenv("XALT_SCALAR_AND_SPSR_SAMPLING");
    }
  if (xalt_sampling == NULL || strcmp(xalt_sampling,"yes") != 0)
    xalt_sampling = "no";
  insert_key_string(&qaT,"XALT_SAMPLING", xalt_sampling);

  if (strcasecmp(transmission, "file") == 0 || strcasecmp(transmission, "file_separate_dirs") == 0)
    {
      build_resultDir(&resultDir, "run", transmission, uuid_str);
      build_resultFn( &resultFn,  "run", start_time, syshost, uuid_str, suffix);

      insert_key_string(&qaT,"XALT_FILE_PREFIX", XALT_FILE_PREFIX);
      insert_key_string(&qaT,"XALT_RESULT_DIR",  resultDir);
      insert_key_string(&qaT,"XALT_RESULT_FILE", resultFn);
    }


  //*********************************************************************
  // So build the Json table string
  
  char*  jsonStr;
  Json_t json;
  const char* my_sep = blank0;
  json_init(Json_TABLE, &json);
  json_add_char_str( &json, my_sep, "crc",          "0xFFFF");     my_sep = comma;
  json_add_json_str( &json, my_sep, "cmdlineA",      usr_cmdline);
  json_add_char_str( &json, my_sep, "hash_id",       &sha1buf[0]);
  json_add_libT(     &json, my_sep, "libA",          libT);
  json_add_ptA(      &json, my_sep, "ptA",           ptA);
  json_add_S2S(      &json, my_sep, "envT",          envT);
  json_add_S2S(      &json, my_sep, "userT",         userT);
  json_add_S2D(      &json, my_sep, "userDT",        userDT);
  json_add_S2S(      &json, my_sep, "xaltLinkT",     recordT);
  json_add_S2D(      &json, my_sep, "XALT_measureT", measureT);
  json_add_S2S(      &json, my_sep, "XALT_qaT",      qaT);
  json_fini(         &json, &jsonStr);

  crc crcValue = crcFast(jsonStr,strlen(jsonStr));
  char crcStr[7];
  sprintf(&crcStr[0],"0x%04X",crcValue);
  memcpy(&jsonStr[8],crcStr,6);

  DEBUG(my_stderr,"    Built json string\n");

  processTreeFree(&ptA);
  free_S2S(&qaT);
  free_S2D(&measureT);
  free_S2D(&userDT);
  free_S2S(&userT);
  free_S2S(&envT);
  free_S2S(&recordT);
  free_SET(&libT);

  char key[50];
  sprintf(&key[0], "%s%s", (end_record) ? "run_fini_" : "run_strt_", uuid_str);

  DEBUG(my_stderr,"    Transmitting jsonStr\n");
  transmit(transmission, jsonStr, "run", key, crcStr, syshost, resultDir, resultFn, my_stderr);
  DEBUG(my_stderr,"    Done transmitting jsonStr\n");
  xalt_quotestring_free();
  my_free(jsonStr, strlen(jsonStr));
  if (resultFn)
    {
      my_free(resultFn , strlen(resultFn)); 
      my_free(resultDir, strlen(resultDir));
    }

  if (strcmp(xalt_kind,"PKGS") == 0)
    {
      DEBUG(my_stderr,"    Transmitting pkg records\n");
      pkgRecordTransmit(uuid_str, syshost, transmission, my_stderr);
      DEBUG(my_stderr,"    Done transmitting pkg records\n");
    }

  my_free(syshost, strlen(syshost));
  DEBUG(my_stderr,"  }\n\n");
  if (xalt_tracing && my_stderr)
    fflush(my_stderr);
}

void pkgRecordTransmit(const char* uuid_str, const char* syshost, const char* transmission, FILE* my_stderr)
{
  char * xalt_tmpdir = create_xalt_tmpdir_str(uuid_str);
  DIR*   dirp        = opendir(xalt_tmpdir);
  if (dirp == NULL)
    {
      my_free(xalt_tmpdir, strlen(xalt_tmpdir));
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
  char crcStr[7];

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
              my_free(buf,sz);
              sz = 0; buf = NULL;

              // build key from dp->d_name;
              //                                                                           0123456789 1234567
              //pkg.rios.2018_11_06_16_14_13_7992.user.d20188d7-bbbb-4b91-9f5c-80672045c270.3ee8e5affda9.json
              int my_len = strlen(dp->d_name);
              utstring_printf(key,"pkg_%s_%.*s",uuid_str, ulen, &dp->d_name[my_len - 17]);
              
              // extract crc string from jsonStr:
              // the crc string must be at the begining of jsonStr
              char * s = utstring_body(jsonStr);
              memcpy(crcStr, &s[8], 6);
              crcStr[6] = '\0';

              // transmit jsonStr
              transmit(transmission, utstring_body(jsonStr), "pkg", utstring_body(key), crcStr, syshost,
                       resultDir, dp->d_name, my_stderr);
              unlink(utstring_body(fullName));
            }
          fclose(fp);
        }
    }
  utstring_free(jsonStr);
  utstring_free(fullName);
  utstring_free(key);
  rmdir(xalt_tmpdir);
  if (resultDir)
    my_free(resultDir, strlen(resultDir));  
  my_free(xalt_tmpdir, strlen(xalt_tmpdir));
}
