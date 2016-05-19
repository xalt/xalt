#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <strings.h>

#include "Options.h"
#include "Json.h"
#include "xalt_config.h"
#include "capture.h"
#include "transmit.h"
#include "buildRmapT.h"
#include "run_submission.h"
#include "run_direct2db.h"

#define DATESZ 100

int main(int argc, char* argv[], char* env[])
{
  Options options(argc, argv);
  char    dateStr[DATESZ];
  time_t  time;
  char*   p;


  //*********************************************************************
  // Build the env table:
  Table envT;
  buildEnvT(env, envT);

  //*********************************************************************
  // Build userT
  Table userT;
  buildUserT(options, userT);

  //*********************************************************************
  // Extract the xalt record stored in the executable (possibly)
  Table recordT;
  extractXALTRecord(options.exec(), recordT);

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

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

  if (strcasecmp(transmission, "direct2db") == 0)
    {
      Vstring xlibmapA;
      Table   rmapT;
      buildRmapT(rmapT, xlibmapA);
      run_direct2db(options.confFn().c_str(), options.userCmdLine(), sha1_exec, rmapT, envT, userT, recordT, libA);
      return 0;
    }
  
  //*********************************************************************
  // If here then we need the json string.  So build it!

  Json    json;
  json.add_json_string("cmdlineA",options.userCmdLine());
  json.add("envT",envT);
  json.add("userT",userT);
  json.add("xaltLinkT",recordT);
  json.add("hash_id",sha1_exec);
  json.add("libA",libA);
  json.fini();

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
      p = getenv("HOME");
      if (p != NULL)
        {
          std::string home   = p;
          std::string suffix = (options.endTime() > 0.0) ? "zzz" : "aaa";
  
          time = options.startTime();
          strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));

          std::ostringstream sstream;
          sstream << home << "/.xalt.d/run." << options.syshost() << ".";
          sstream << dateStr << "." << suffix << "." << options.uuid() << ".json";

          fn = sstream.str();
          resultFn = fn.c_str();
        }
    }

  transmit(transmission, jsonStr, "run", key, options.syshost().c_str(), resultFn);
  return 0;
}
