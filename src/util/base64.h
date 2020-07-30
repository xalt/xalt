#ifndef BASE64_H
#define BASE64_H

#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif

char*          base64_encode(const void* binaryData, int len, int *flen);
unsigned char* base64_decode(const char* ascii,      int len, int *flen);

#ifdef __cplusplus
}
#endif

#endif //BASE64_H
