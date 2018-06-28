#ifndef XALT_C_UTILS_H
#define XALT_C_UTILS_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <sys/stat.h>

int   isDirectory(const char *path);
int   mkpath(char *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif //XALT_C_UTILS_H
