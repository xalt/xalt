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

  Json json;
  

  
  

  free(my_realpath);

  return 0;
}
