#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <strings.h>
#include <string.h>

#include "walkProcessTree.h"
#include "Options.h"
#include "Json.h"
#include "xalt_config.h"
#include "capture.h"
#include "transmit.h"
#include "buildRmapT.h"
#include "run_submission.h"
#include "run_direct2db.h"
#include "xalt_utils.h"

#define DATESZ 100

int main(int argc, char* argv[], char* env[])
{

  char * p_dbg        = getenv("XALT_TRACING");
  int    xalt_tracing = (p_dbg && strcmp(p_dbg,"yes") == 0);

  DEBUG0(stderr,"\nxalt_run_submission() {\n");

  Options options(argc, argv);
  char    dateStr[DATESZ];
  time_t  time;
  char*   p;

  walkProcessTree(options.ppid());

  walkProcessTree(options.ppid());


  //*********************************************************************
  // Build the env table:
  Table envT;
  buildEnvT(env, envT);
  DEBUG0(stderr,"  Built envT\n");

  //*********************************************************************
  // Extract the xalt record stored in the executable (possibly)
  Table recordT;
  extractXALTRecord(options.exec(), recordT);
  DEBUG0(stderr,"  Extracted recordT from executable\n");

  //*********************************************************************
  // Build userT
  Table  userT;
  DTable userDT;

  buildUserT(options, envT, userT, userDT);
  userDT["Build_Epoch"] = strtod(recordT["Build_Epoch"],(char **) NULL);
  DEBUG0(stderr,"  Built userT, userDT\n");

  //*********************************************************************
  // Take sha1sum of the executable
  std::vector<std::string> result;
  std::string              cmd;
  cmd  = SHA1SUM " " + options.exec();
  capture(cmd, result);
  std::string sha1_exec = result[0].substr(0, result[0].find(" "));
  
  //*********************************************************************
  // Parse the output of ldd for this executable.
  std::vector<Libpair> libA;
  parseLDD(options.exec(), libA);
  DEBUG0(stderr,"  Parsed LDD\n");

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;
  
  DEBUG1(stderr,"  Using XALT_TRANSMISSION_STYLE: %s\n",transmission);


  if (strcasecmp(transmission, "direct2db") == 0)
    {
      std::string rmapD = ""; // must use XALT_ETC_DIR to specify where rmapD is!
      Table       rmapT;
      Vstring     xlibmapA;

      buildRmapT(rmapD, rmapT, xlibmapA);
      run_direct2db(options.confFn().c_str(), options.userCmdLine(), sha1_exec, rmapT, envT, userT, userDT, recordT, libA);
      DEBUG0(stderr,"  Completed run direct to DB\n");
      DEBUG0(stderr,"}\n\n");
      
      return 0;
    }
  
  //*********************************************************************
  // If here then we need the json string.  So build it!

  Json json;
  json.add_json_string("cmdlineA",options.userCmdLine());
  json.add("envT",envT);
  json.add("userT",userT);
  json.add("userDT",userDT);
  json.add("xaltLinkT",recordT);
  json.add("hash_id",sha1_exec);
  json.add("libA",libA);
  json.fini();

  DEBUG0(stderr,"  Built json string\n");

  std::string jsonStr = json.result();
  std::string fn;
  const char* resultFn = NULL;


  std::string key   = ((options.endTime() > 0.0) ? "run_fini_" : "run_strt_");
  key.append(options.uuid());

  if (strcasecmp(transmission, "file") == 0)
    {
      //*********************************************************************
      // Build file name for xalt json record.  It is only used when the
      // file transmission style is used;
      char* c_home = getenv("HOME");
      char* c_user = getenv("USER");
      if (c_home != NULL && c_user != NULL )
        {
          std::string home   = c_home;
          std::string user   = c_user;
          std::string xaltDir;

          build_xaltDir(xaltDir, user, home);

          std::string suffix = (options.endTime() > 0.0) ? "zzz" : "aaa";
  
          time = options.startTime();
          strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));

          std::ostringstream sstream;
          sstream << xaltDir << "run." << options.syshost() << ".";
          sstream << dateStr << "." << suffix << "." << options.uuid() << ".json";

          fn = sstream.str();
          resultFn = fn.c_str();
        }
    }

  transmit(transmission, jsonStr, "run", key, options.syshost().c_str(), resultFn);
  DEBUG0(stderr,"}\n\n");
  return 0;
}
