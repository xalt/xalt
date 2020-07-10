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
#include "xalt_utils.h"
#include "xalt_c_utils.h"
#include "link_submission.h"
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

  std::vector<Libpair> libA;
  parseLDTrace(xaltobj, linklineFn, libA);

  Set funcSet;
  readFunctionList(funcRawFn, funcSet);

  std::string compiler;
  std::string compilerPath;
  Vstring     linklineA;
  parseCompTJsonStr("COMP_T", compT, compiler, compilerPath, linklineA);

  S2S_t* recordT  = NULL;

  const char* user = getenv("USER");
  if (user == NULL)
    user = "unknown";

  insert_key_string(&resultT, "uuid",          uuid);
  insert_key_string(&resultT, "link_program",  compiler);
  insert_key_string(&resultT, "link_path",     compilerPath);
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

  json_add_S2S(&json, my_sep, "resultT",  resultT); my_sep = comma
  json.add("linkA",    libA);
  json.add("function", funcSet);
  json.add("link_line",linklineA);
  json.fini();

  std::string jsonStr = json.result();
  std::string key("link_");
  key.append(uuid);

  char* resultDir = NULL;
  char* resultFn  = NULL;

  if (strcasecmp(transmission, "file") == 0 || strcasecmp(transmission, "file_separate_dirs") == 0)
    {
      build_resultDir(&resultDir, "link", transmission, uuid);
      build_resultFn( &resultFn,  start, syshost, uuid, "link", "");
    }

  transmit(transmission, jsonStr.c_str(), "link", key.c_str(), syshost, resultDir, resultFn);
  free(resultDir);
  free(resultFn);
  return 0;
}
