#ifndef PARSEJSONSTR_H
#define PARSEJSONSTR_H

#include "jsmn.h"
#include "utarray.h"

void parseCompTJsonStr(const char* name, std::string& jsonStr, std::string& compiler, std::string& compilerPath,
                       UT_array** linklineA);


#endif // PARSEJSONSTR_H
