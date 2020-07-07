#ifndef PROCESSTREE_H
#define PROCESSTREE_H

#include <sys/types.h>
#include <unistd.h>
#include "utstring.h"
#include "utarray.h"

typedef struct processTree_t {
  UT_string* m_path;
  UT_string* m_name;
  UT_array*  m_cmdlineA;
  pid_t      m_pid;

  struct processTree_t *next, *prev;
} processTree_t;

void walkProcessTree(pid_t pid, processTree_t** ptA);

#endif //PROCESSTREE_H
