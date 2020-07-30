#ifndef ZSTRING_H
#define ZSTRING_H

#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif

char* compress_string(  const char* in, int* lenOut);
char* uncompress_string(const char* in, int  lenOut);

#ifdef __cplusplus
}
#endif



#endif //ZSTRING_H
