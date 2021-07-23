#ifndef XALT_C_UTILS_H
#define XALT_C_UTILS_H

#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

  int  isDirectory(const char *path);
  int  mkpath(char *path, mode_t mode);
  void build_resultDir(char **resultDir, const char* kind, const char* transmission, const char* uuid_str);
  void build_resultFn( char** resultFn,  const char* kind, double start, const char* syshost, const char* uuid,
                      const char* suffix);

  const char* xalt_file_transmission_method();
  void set_end_record();
  void my_free(void* ptr,int sz);
#ifdef __cplusplus
}
#endif

#endif //XALT_C_UTILS_H
