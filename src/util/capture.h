#ifndef CAPTURE_H
#define CAPTURE_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "utarray.h"
void capture(const char* cmd, UT_array** p_resultA);

#ifdef __cplusplus
}
#endif
#endif //CAPTURE_H
