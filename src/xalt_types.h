#ifndef XALT_TYPES_H
#define XALT_TYPES_H

#include "xalt_base_types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sys/types.h>


typedef std::vector<std::string>                    Vstring;
typedef std::unordered_set<std::string>             Set;
typedef std::unordered_map<std::string,std::string> Table;
typedef std::unordered_map<const char*,const char*> CTable;
typedef std::unordered_map<std::string,double>      DTable;
typedef unsigned int                                uint;

struct Libpair
{
  Libpair(const std::string& libIn, const std::string& sha1In)
    : lib(libIn), sha1(sha1In) {}

  std::string lib;
  std::string sha1;
};

struct ProcessTree
{
  ProcessTree(pid_t pidIn, const std::string& nameIn, const std::string& pathIn, Vstring& cmdlineIn)
    : name(nameIn), path(pathIn), cmdlineA(cmdlineIn), pid(pidIn){}

  std::string name;
  std::string path;
  Vstring     cmdlineA;
  pid_t       pid;
};

#endif //XALT_TYPES_H
