#ifndef XALT_FGETS_ALLOC_H
#define XALT_FGETS_ALLOC_H

#include <stdio.h>
#include "xalt_function_names.h"

#ifdef __cplusplus
extern "C" {
#endif

int xalt_fgets_alloc(FILE *fp, char ** buffer, size_t* sz);

#ifdef __cplusplus
}
#endif

#endif  //XALT_FGETS_ALLOC_H
