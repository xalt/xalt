#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include "xalt_types.h"
#include "Json.h"
#include "xalt_config.h"
#include "extract_linker.h"
#include "transmit.h"
#include "capture.h"
#include "buildRmapT.h"
#include "xalt_utils.h"
#include "link_submission.h"
#include <sys/stat.h>
#include <sys/types.h>

#define HERE fprintf(stderr,"%s:%d\n", __FILE__, __LINE__)

const int syslog_msg_sz = 512;
int main(int argc, char* argv[])
{
  const char* uuid        = argv[ 1];
  const char* status      = argv[ 2];
  const char* wd          = argv[ 3];
  const char* syshost     = argv[ 4];
  const char* execname    = argv[ 5];
  const char* xaltobj     = argv[ 6];
  const char* build_epoch = argv[ 7];
  const char* funclistFn  = argv[ 8];
  const char* linklineFn  = argv[ 9];
  const char* resultFn    = argv[10];


  if (strstr(execname,"conftest") != NULL)
    return 1;

  Vstring     result;
  std::string cmd = SHA1SUM " ";
  cmd.append(execname);
  
  capture(cmd, result);
  std::string sha1_exec = result[0].substr(0, result[0].find(" "));

  
  std::vector<Libpair> libA;
  parseLDTrace(xaltobj, linklineFn, libA);

  Set funcSet;
  readFunctionList(funclistFn, funcSet);

  std::string compiler;
  std::string compilerPath;
  Vstring     linklineA;
  extract_linker(compiler, compilerPath, linklineA);

  Table resultT;

  const char* user = getenv("USER");
  if (user == NULL)
    user = "unknown";

  char* my_realpath = canonicalize_file_name(execname);
  cmd = SHA1SUM " ";
  cmd.append(my_realpath);
  capture(cmd,result);
  std::string sha1 = result[0].substr(0, result[0].find(" "));
  free(my_realpath);

  resultT["uuid"]          = uuid;
  resultT["link_program"]  = compiler;
  resultT["link_path"]     = compilerPath;
  resultT["build_user"]    = user;
  resultT["build_epoch"]   = build_epoch;
  resultT["exit_code"]     = status;
  resultT["exec_path"]     = my_realpath;
  resultT["hash_id"]       = sha1;
  resultT["wd"]            = wd;
  resultT["build_syshost"] = syshost;

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

  if (strcasecmp(transmission,"direct2db") == 0)
    {
      Vstring xlibmapA;
      Table   rmapT;
      buildRmapT(rmapT, xlibmapA);

      link_direct2db(linklineA, resultT, libA, funcSet, rmapT);
      return 0;
    }
  
  Json json;
  json.add("resultT",resultT);
  json.add("linkA",libA);
  json.add("function",funcSet);
  json.add("link_line",linklineA);
  json.fini();

  std::string jsonStr = json.result();
  std::string key("link_");
  key.append(uuid);


  // if user has wiped out $HOME then set resultFn to NULL so that file transmission will do nothing!
  const char* home = getenv("HOME");
  if (home == NULL)
    resultFn == NULL;

  // transmit results to anything that is not "direct2db"
  transmit(transmission, jsonStr, "link", key, resultFn);

  return 0;
}
