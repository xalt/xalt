#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "capture.h"
#include "Json.h"
#include "xalt_config.h"
#include "xalt_types.h"

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


  if (strstr(execname,"conftest") == NULL)
    return 1;

  

  std::vector<std::string> result;
  std::string cmd = SHA1SUM " " + execname;
  capture(cmd, result);
  std::string sha1_exec = result[0].substr(0, result[0].find(" "));

  
  std::vector<Libpair> libA;
  parseLDTrace(linklineFn, libA);

  Set funcSet;
  readFunctionList(funclistFn, funcSet);

  std::string compiler;
  std::string compilerPath;
  std::vector<std::string> linklineA;
  extract_linker(compiler, compilerPath, linklineA);

  Table resultT;

  const char* user = getenv("USER");
  if (user == NULL)
    user = "unknown";

  char* my_realpath = canonicalize_file_name(execname);
  std::vector<std::string> result;
  std::string cmd = SHA1SUM " ";
  cmd.append(my_realpath);
  capture(cmd,result);
  std::string sha1 = result[0].substr(0, result[0].find(" "));
  free(my_realpath);

  resultT["uuid"]          = uuid;
  resultT["link_program"]  = compiler;
  resultT["link_path"]     = compilerPath;
  resultT["build_user"]    = user
  resultT["build_epoch"]   = build_epoch;
  resultT["exit_code"]     = status;
  resultT["exec_path"]     = my_realpath;
  resultT["hash_id"]       = sha1;
  resultT["wd"]            = wd;
  resultT["build_syshost"] = syshost;

  const char * trans = getenv("XALT_TRANSMISSION_STYLE");
  if (trans == NULL)
    trans = TRANSMISSION;
  std::string transmission(trans);
  std::transform(transmission.begin(), transmission.end(), transmission.begin(), ::tolower);

  if (transmission == "direct2db")
    {
      // fix me
      return 0;
    }
  
  Json json;
  json.add("resultT",resultT);
  json.add("linkA",libA);
  json.add("function",funcSet);
  json.add("link_line",linklineA);
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
  
      time = options.startTime();
      strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));

      std::ostringstream sstream;
      sstream << home << "/.xalt.d/link." << options.syshost() << ".";
      sstream << dateStr << "." << options.uuid() << ".json";

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
      cmd << LOGGER " -t XALT_LOGGING \"link:" << options.syshost();
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

      std::string key   = "link_" + options.uuid();

      for (int i = 0; i < nBlks; ++i)
        {
          cmd << LOGGER " -t XALT_LOGGING V:2 kind:link idx:" << i;
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
