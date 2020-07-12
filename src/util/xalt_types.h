#ifndef XALT_TYPES_H
#define XALT_TYPES_H

#include "xalt_base_types.h"
#include "utstring.h"
#include "uthash.h"
#include "utarray.h"

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

typedef struct processTree_t {
  UT_string* m_path;
  UT_string* m_name;
  UT_array*  m_cmdlineA;
  pid_t      m_pid;

  struct processTree_t *next, *prev;
} processTree_t;




#endif //XALT_TYPES_H
