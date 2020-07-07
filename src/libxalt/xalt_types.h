#ifndef XALT_TYPES_H
#define XALT_TYPES_H

#include "utstring.h"
#include "uthash.h"

typedef struct {
  UT_string*     key;
  UT_string*     value;
  UT_hash_handle hh;
} S2S_t;

typedef struct {
  UT_string*     key;
  double         value;
  UT_hash_handle hh;
} S2D_t;

typedef struct {
  UT_string*     key;
  UT_hash_handle hh;
} SET_t;




#endif //XALT_TYPES_H
