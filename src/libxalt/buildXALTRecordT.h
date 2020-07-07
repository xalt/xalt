#ifndef BUILD_XALT_RECORDT_H
#define BUILD_XALT_RECORDT_H

#include "xalt_types.h"

void buildXALTRecordT(char* watermark, S2S_t** recordT);
void extractXALTRecord(const char* exec_path, char** watermark);

#endif //BUILD_XALT_RECORDT_H
