#ifndef INSERT_H
#define INSERT_H

#include "xalt_types.h"
#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif

  void insert_key_double(S2D_t** userDT, const char* name, double      value);
  void insert_key_string(S2S_t** userT,  const char* name, const char* value);
  void insert_key_SET(   SET_t** userT,  const char* name);
  
#ifdef __cplusplus
}
#endif

#endif //INSERT_H
