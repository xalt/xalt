#ifndef LINK_SUBMISSION_H
#define LINK_SUBMISSION_H


#include "xalt_types.h"
void parseLDTrace(const char* xaltobj, const char* linkfileFn, std::vector<Libpair>& libA);
void readFunctionList(const char* fn, Set& funcSet);

#endif //LINK_SUBMISSION_H
