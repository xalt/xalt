#include <iostream>
#include <sstream>
#include <time.h>
#include "Options.h"
#include "Json.h"

#define DATESZ 100

int main(int argc, char* argv[], char* env[])
{
  Options options(argc, argv);
  char    dateStr[DATESZ];
  time_t  time;
  char*   p;
  Json    json;

  //*********************************************************************
  // Print the user command line.
  json.add_json_string("cmdline:",options.userCmdLine());

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
  //std::cout << sstream.str() << "\n";

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
  cmd  = "sha1sum ";
  cmd += options.exec();
  capture(cmd, result);
  std::string sha1_exec = result[0].substr(0, result[0].find(" "));
  
  json.add("hash_id",sha1_exec);

  //*********************************************************************
  // Parse the output of ldd for this executable.
  std::vector<Libpair> libA;
  parseLDD(options.exec(), libA);
  json.add("libA",libA);

  json.fini();
  std::cout << json.result() << std::endl;

  return 0;
}
