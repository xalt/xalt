#ifndef PARSE_LD_TRACE_H
#define PARSE_LD_TRACE_H


#include "xalt_types.h"
void parseLDTrace(const char* xaltobj, const char* linkfileFn, SET_t** libA);
void readFunctionList(const char* fn,  SET_t** funcSet);

#endif //PARSE_LD_TRACE_H
