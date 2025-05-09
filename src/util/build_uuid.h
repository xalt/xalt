#ifndef BUILD_UUID_H
#define BUILD_UUID_H

#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif

  void build_uuid(char * my_uuid_str);
  int have_libc_getentropy_func();

#ifdef __cplusplus
}
#endif

#endif  /*BUILD_UUID_H*/
