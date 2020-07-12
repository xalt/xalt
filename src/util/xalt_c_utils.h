#ifndef XALT_C_UTILS_H
#define XALT_C_UTILS_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <sys/stat.h>

  int  isDirectory(const char *path);
  int  mkpath(char *path, mode_t mode);
  void build_resultDir(char **resultDir, const char* kind, const char* transmission, const char* uuid_str);
  void build_resultFn( char** resultFn,  const char* kind, double start, const char* syshost, const char* uuid,
                      const char* suffix);

#ifdef __cplusplus
}
#endif

#endif //XALT_C_UTILS_H
