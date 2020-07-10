#ifndef PARSEJSONSTR_H
#define PARSEJSONSTR_H

#include "xalt_cxx_types.h"
#include "jsmn.h"

void parseCompTJsonStr(const char* name, std::string& jsonStr, std::string& compiler, std::string& compilerPath,
                       Vstring& linklineA);


#endif // PARSEJSONSTR_H
