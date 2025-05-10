#ifndef BUILD_UUID_H
#define BUILD_UUID_H

#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif

  int  build_uuid(char * my_uuid_str);
  void build_uuid_cleanup();
  int  have_libc_getentropy_func();

#ifdef __cplusplus
}
#endif

#endif  /*BUILD_UUID_H*/
