#ifndef BUILD_RESULTFN_H
#define BUILD_RESULTFN_H

#include <string>
#include "Options.h"

void build_resultFn(std::string& resultDir, std::string& resultFn, double start, const char* syshost, const char* uuid,
                    const char *kind, const char* transmission);

#endif // BUILD_RESULTFN_H
