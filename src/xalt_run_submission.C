#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <strings.h>
#include <string.h>

#include "xalt_quotestring.h"
#include "epoch.h"
#include "walkProcessTree.h"
#include "Options.h"
#include "Json.h"
#include "xalt_config.h"
#include "transmit.h"
#include "buildRmapT.h"
#include "run_submission.h"
//#include "run_direct2db.h"
#include "xalt_utils.h"

#define DATESZ 100

int main(int argc, char* argv[], char* env[])
{
  char * p_dbg        = getenv("XALT_TRACING");
  int    xalt_tracing = (p_dbg && ( strcmp(p_dbg,"yes") == 0 || strcmp(p_dbg,"run") == 0));

  Options options(argc, argv);
  char    dateStr[DATESZ];
  time_t  time;
  double  t0, t1;
  double  t_ldd, t_sha1;
  DTable  measureT;
  bool    end_record = (options.endTime() > 0.0);
  
  std::string suffix = end_record ? "zzz" : "aaa";
  DEBUG1(stderr,"\nxalt_run_submission(%s) {\n",suffix.c_str());
  
  t0 = epoch();
  t1 = t0;
  std::vector<ProcessTree> ptA;
  walkProcessTree(options.ppid(), ptA);
  measureT["04_WalkProcTree_"] = epoch() - t1;
    
  //*********************************************************************
  // Build the env table:
  t1 = epoch();
  Table envT;
  buildEnvT(options, env, envT);
  DEBUG0(stderr,"  Built envT\n");
  measureT["03_BuildEnvT____"] = epoch() - t1;

  //*********************************************************************
  // Extract the xalt record stored in the executable (possibly)
  t1 = epoch();
  Table recordT;
  extractXALTRecord(options.exec(), recordT);
  DEBUG0(stderr,"  Extracted recordT from executable\n");
  measureT["05_ExtractXALTR_"] = epoch() - t1;

  //*********************************************************************
  // Build userT
  t1 = epoch();
  Table  userT;
  DTable userDT;

  buildUserT(options, envT, userT, userDT);
  if ( ! recordT.empty())
    userDT["Build_Epoch"] = strtod(recordT["Build_Epoch"].c_str(),(char **) NULL);
  DEBUG0(stderr,"  Built userT, userDT\n");
  measureT["01_BuildUserT___"] = epoch() - t1;

  //*********************************************************************
  // Filter envT 
  t1 = epoch();
  filterEnvT(env, envT);
  DEBUG0(stderr,"  Filter envT\n");
  measureT["03_BuildEnvT____"] += epoch() - t1;
  

  //*********************************************************************
  // Take sha1sum of the executable
  t1 = epoch();
  std::string sha1_exec;
  compute_sha1(options.exec(), sha1_exec);

  measureT["02_Sha1_exec____"] = epoch() - t1;
  
  //*********************************************************************
  // Parse the output of ldd for this executable (start record only)
  std::vector<Libpair> libA;
  parseLDD(options.exec(), libA, t_ldd, t_sha1);
  DEBUG0(stderr,"  Parsed LDD\n");

  measureT["06_ParseLDD_ldd_"] = t_ldd;
  measureT["06_ParseLDD_sha1"] = t_sha1;
  
  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;
  
  DEBUG1(stderr,"  Using XALT_TRANSMISSION_STYLE: %s\n",transmission);

  //if (strcasecmp(transmission, "direct2db") == 0)
  //  {
  //    std::string rmapD = ""; // must use XALT_ETC_DIR to specify where rmapD is!
  //    Table       rmapT;
  //    Vstring     xlibmapA;
  //
  //    buildRmapT(rmapD, rmapT, xlibmapA);
  //    run_direct2db(options.confFn().c_str(), options.userCmdLine(), sha1_exec, rmapT, envT, userT, userDT, recordT, libA);
  //    DEBUG0(stderr,"  Completed run direct to DB\n");
  //    DEBUG0(stderr,"}\n\n");
  //    
  //    return 0;
  //  }
  
  //*********************************************************************
  // If here then we need the json string.  So build it!
  measureT["07____total_____"] = epoch() - t0;

  Json json;
  json.add_json_string("cmdlineA",options.userCmdLine());
  json.add("ptA", ptA);
  json.add("envT",envT);
  json.add("userT",userT);
  json.add("userDT",userDT);
  json.add("xaltLinkT",recordT);
  json.add("hash_id",sha1_exec);
  json.add("libA",libA);
  json.add("XALT_measureT",measureT);
  json.fini();

  DEBUG0(stderr,"  Built json string\n");

  std::string jsonStr = json.result();
  std::string fn;
  char* resultFn = NULL;


  std::string key   = (end_record) ? "run_fini_" : "run_strt_";
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

          double start = options.startTime();
          double frac  = start - floor(start);
          time = options.startTime();
          strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));

          std::ostringstream sstream;
          sstream << xaltDir << "run." << options.syshost() << ".";
          sstream << dateStr << "_" << std::setfill('0') << std::setw(4) << (int) (frac*10000.0) << "."
                  << suffix << "." << options.uuid() << ".json";

          fn = sstream.str();
          resultFn = strdup(fn.c_str());
        }
    }

  transmit(transmission, jsonStr.c_str(), "run", key.c_str(), options.syshost().c_str(), resultFn);
  xalt_quotestring_free();
  if (resultFn)
    free(resultFn);
  DEBUG0(stderr,"}\n\n");
  if (xalt_tracing)
    fflush(stderr);
  return 0;
}
