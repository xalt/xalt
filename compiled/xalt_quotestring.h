#ifndef QUOTESTRING_H
#define QUOTESTRING_H

#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C" {
#endif

  const char* xalt_quotestring(  const char* input);
  const char* xalt_unquotestring(const char* input, int len);

#ifdef __cplusplus
}
#endif

#endif //QUOTESTRING_H
