#ifndef TRANSMIT_H
#define TRANSMIT_H

#include <stdio.h>
#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif

void transmit(const char* transmission, const char* jsonStr, const char* kind, const char* key,
              const char* crcStr, const char* syshost, char* resultDir, const char* resultFn,
              FILE* my_stderr);

#ifdef __cplusplus
}
#endif

#endif //TRANSMIT_H
