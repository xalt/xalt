#ifndef BUILD_XALT_RECORDT_H
#define BUILD_XALT_RECORDT_H

#include <stdbool.h>
#include "xalt_types.h"
#include "xalt_obfuscate.h"

void buildXALTRecordT(char* watermark, S2S_t** recordT);
bool extractXALTRecordString(const char* exec_path, char** watermark);

#endif //BUILD_XALT_RECORDT_H
