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
#include <algorithm>

#define DATESZ 100
const int syslog_msg_sz = 512;

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


  const char * trans = getenv("XALT_TRANSMISSION_STYLE");
  if (trans == NULL)
    trans = TRANSMISSION;
  std::string transmission(trans);
  std::transform(transmission.begin(), transmission.end(), transmission.begin(), ::tolower);

  if (transmission == "direct2db")
    {
      direct2db(usr_cmdline, hash_id, rmapT, envT, userT, recordT, lddA);
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

  if (strcasecmp(transmission, "file") == 0)
    {
      //*********************************************************************
      // Build file name for xalt json record.  It is only used when the
      // file transmission style is used;
      p = getenv("HOME");
      if (p == NULL)
        return 0;     
      std::string home   = p;
      std::string suffix = (options.endTime() > 0.0) ? "zzz" : "aaa";
  
      time = options.startTime();
      strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));

      std::ostringstream sstream;
      sstream << home << "/.xalt.d/run." << options.syshost() << ".";
      sstream << dateStr << "." << suffix << "." << options.uuid() << ".json";

      std::ofstream myfile;
      myfile.open(sstream.str());
      myfile << json.result();
      myfile.close();
    }
  else if (strcasecmp(transmission, "syslogv1") == 0)
    {
      std::ostringstream cmd;
      std::string jsonStr = json.result();
      std::string b64     = base64_encode(reinterpret_cast<const unsigned char*>(jsonStr.c_str()), jsonStr.size());
      cmd << LOGGER " -t XALT_LOGGING \"run:" << options.syshost();
      cmd << ":" << b64 << "\"";
      system(cmd.str().c_str());
    }
  else if (strcasecmp(transmission, "syslog") == 0)
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
    }
  return 0;
}
