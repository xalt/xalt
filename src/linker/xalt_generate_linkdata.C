#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "xalt_types.h"
#include "xalt_cxx_types.h"
#include "buildJson.h"
#include "xalt_config.h"
#include "transmit.h"
//include "xalt_utils.h"
#include "xalt_c_utils.h"
#include "parseLDTrace.h"
#include "insert.h"
#include "parseJsonStr.h"

static const char* blank0 = "";
static const char* comma  = ",";

double convert_double(const char* name, const char* s)
{
  char  *p;
  double v;

  v = strtod(s, &p);
  if (p == s || *p)
    {
      fprintf(stderr,"For option: \"%s\", unable to parse: \"%s\"\n", name, s);
      exit(1);
    }
  return v;
}

int main(int argc, char* argv[])
{
  int         i           = 1;
  const char* uuid        = argv[i++];
  const char* wd          = argv[i++];
  const char* syshost     = argv[i++];
  const char* execname    = argv[i++];
  const char* sha1sum     = argv[i++]; 
  const char* xaltobj     = argv[i++];
  const char* build_epoch = argv[i++];
  const char* funcRawFn   = argv[i++];
  const char* linklineFn  = argv[i++];
  std::string compT       = argv[i++];

  if (compT.size() == 0)
    compT = "{}";

  double start = convert_double("startTime",build_epoch);

  if (strstr(execname,"conftest") != NULL)
    return 1;

  SET_t* libT = NULL;
  parseLDTrace(xaltobj, linklineFn, &libT);

  SET_t* funcSet=NULL;
  readFunctionList(funcRawFn, &funcSet);

  std::string compiler;
  std::string compilerPath;
  UT_array*   linklineA;
  utarray_new(linklineA, &ut_str_icd);
  parseCompTJsonStr("COMP_T", compT, compiler, compilerPath, &linklineA);

  S2S_t* resultT  = NULL;

  const char* user = getenv("USER");
  if (user == NULL)
    user = "unknown";

  insert_key_string(&resultT, "uuid",          uuid);
  insert_key_string(&resultT, "link_program",  compiler.c_str());
  insert_key_string(&resultT, "link_path",     compilerPath.c_str());
  insert_key_string(&resultT, "build_user",    user);
  insert_key_string(&resultT, "build_epoch",   build_epoch);
  insert_key_string(&resultT, "exec_path",     execname);
  insert_key_string(&resultT, "hash_id",       sha1sum);
  insert_key_string(&resultT, "wd",            wd);
  insert_key_string(&resultT, "build_syshost", syshost);

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

  char*       jsonStr;
  Json_t      json;
  const char* my_sep = blank0;
  json_init(Json_TABLE, &json);

  json_add_S2S(     &json, my_sep, "resultT",   resultT); my_sep = comma;
  json_add_libT(    &json, my_sep, "linkA",     libT);
  json_add_SET(     &json, my_sep, "function",  funcSet);
  json_add_utarray( &json, my_sep, "link_line", linklineA);
  json_fini(        &json, &jsonStr);

  std::string key("link_");
  key.append(uuid);

  char* resultDir = NULL;
  char* resultFn  = NULL;

  if (strcasecmp(transmission, "file") == 0 || strcasecmp(transmission, "file_separate_dirs") == 0)
    {
      build_resultDir(&resultDir, "link", transmission, uuid);
      build_resultFn( &resultFn,  "link", start, syshost, uuid, "");
    }

  transmit(transmission, jsonStr, "link", key.c_str(), syshost, resultDir, resultFn, stderr);
  memset(jsonStr,   '\0', strlen(jsonStr));   free(jsonStr);
  memset(resultFn,  '\0', strlen(resultFn));  free(resultFn);
  memset(resultDir, '\0', strlen(resultDir)); free(resultDir);
  return 0;
}
