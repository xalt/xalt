#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "xalt_types.h"
#include "Json.h"
#include "xalt_config.h"
#include "transmit.h"
#include "capture.h"
#include "buildRmapT.h"
#include "xalt_utils.h"
//#include "link_direct2db.h"
#include "link_submission.h"
#include "parseJsonStr.h"

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
  char*       resultFn    = argv[i++];
  std::string compT       = argv[i++];

  if (compT.size() == 0)
    compT = "{}";


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

  Table resultT;

  const char* user = getenv("USER");
  if (user == NULL)
    user = "unknown";

  resultT["uuid"]          = uuid;
  resultT["link_program"]  = compiler;
  resultT["link_path"]     = compilerPath;
  resultT["build_user"]    = user;
  resultT["build_epoch"]   = build_epoch;
  resultT["exec_path"]     = execname;
  resultT["hash_id"]       = sha1sum;
  resultT["wd"]            = wd;
  resultT["build_syshost"] = syshost;

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

  //if (strcasecmp(transmission,"direct2db") == 0)
  //  {
  //    std::string rmapD = ""; // Must use XALT_ETC_DIR to specify rmapD location!
  //    Table       rmapT;
  //    Vstring     xlibmapA;
  //    buildRmapT(rmapD, rmapT, xlibmapA);
  //
  //    link_direct2db("xalt_db.conf", linklineA, resultT, libA, funcSet, rmapT);
  //    return 0;
  //  }
  
  Json json;
  json.add("resultT",  resultT);
  json.add("linkA",    libA);
  json.add("function", funcSet);
  json.add("link_line",linklineA);
  json.fini();

  std::string jsonStr = json.result();
  std::string key("link_");
  key.append(uuid);

  #ifndef HAVE_FILE_PREFIX
    // if user has wiped out $HOME then set resultFn to NULL so that file transmission will do nothing!
    const char* home = getenv("HOME");
    if (home == NULL)
      resultFn = NULL;
  #endif

  // transmit results to anything that is not "direct2db"
  transmit(transmission, jsonStr.c_str(), "link", key.c_str(), syshost, resultFn);

  return 0;
}
