#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include "Options.h"
#include "Json.h"
#include "xalt_config.h"
#include "base64.h"
#include <strings.h>
#include "zstring.h"

#define DATESZ 100
const int syslog_msg_sz = 512;

int main(int argc, char* argv[], char* env[])
{
  Options options(argc, argv);
  char    dateStr[DATESZ];
  time_t  time;
  char*   p;
  Json    json;

  //*********************************************************************
  // Print the user command line.
  json.add_json_string("cmdlineA",options.userCmdLine());

  //*********************************************************************
  // Build file name for xalt json record.  It is only used when the
  // file transmission style is used;
  p = getenv("HOME");
  std::string home   = (p) ? p : "/";
  std::string suffix = (options.endTime() > 0.0) ? "zzz" : "aaa";
  
  time = options.startTime();
  strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));

  std::ostringstream sstream;
  sstream << home << "/.xalt.d/run." << options.syshost() << ".";
  sstream << dateStr << "." << suffix << "." << options.uuid() << ".json";

  //*********************************************************************
  // Build the env table:
  std::unordered_map<std::string, std::string> envT;
  buildEnvT(env, envT);
  json.add("envT",envT);


  //*********************************************************************
  // Build userT
  std::unordered_map<std::string, std::string> userT;
  buildUserT(options, userT);
  json.add("userT",userT);

  //*********************************************************************
  // Extract the xalt record stored in the executable (possibly)
  std::unordered_map<std::string, std::string> recordT;
  extractXALTRecord(options.exec(), recordT);
  json.add("xaltLinkT",recordT);

  //*********************************************************************
  // Take sha1sum of the executable
  std::vector<std::string> result;
  std::string              cmd;
  cmd  = SHA1SUM " " + options.exec();
  capture(cmd, result);
  std::string sha1_exec = result[0].substr(0, result[0].find(" "));
  
  json.add("hash_id",sha1_exec);

  //*********************************************************************
  // Parse the output of ldd for this executable.
  std::vector<Libpair> libA;
  parseLDD(options.exec(), libA);
  json.add("libA",libA);

  json.fini();

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

  if (strcasecmp(transmission, "file") == 0)
    {
      std::ofstream myfile;
      myfile.open(sstream.str());
      myfile << json.result();
      myfile.close();
      return 0;
    }

  if (strcasecmp(transmission, "syslog") == 0)
    {
      std::ostringstream cmd;
      std::string zs    = compress_string(json.result());
      std::string b64   = base64_encode(reinterpret_cast<const unsigned char*>(zs.c_str()), zs.size());
      int         sz    = b64.size();
      int         blkSz = (sz < syslog_msg_sz) ? sz : syslog_msg_sz;
      int         nBlks = (sz -  1)/blkSz + 1;
      int         istrt = 0;
      int         iend  = blkSz;

      std::string key   = ((options.endTime() > 0.0) ? "run_fini_" : "run_strt_") +
        options.uuid();

      for (int i = 0; i < nBlks; ++i)
        {
          cmd << LOGGER " -t XALT_LOGGING V:2 kind:run idx:" << i;
          cmd << " nb:"  << nBlks << " syshost:" << options.syshost();
          cmd << " key:" << key   << " value:"   << b64.substr(istrt, iend - istrt);
          system(cmd.str().c_str());
          istrt = iend;
          iend  = istrt + blkSz;
          if (iend > sz)
            iend = sz;
        }
      return 0;
    }
  return 0;
}
