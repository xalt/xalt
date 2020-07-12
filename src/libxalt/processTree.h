#ifndef PROCESSTREE_H
#define PROCESSTREE_H

#include <sys/types.h>
#include <unistd.h>
#include "utstring.h"
#include "utarray.h"
#include "xalt_types.h"

void walkProcessTree(pid_t pid, processTree_t** ptA);

#endif //PROCESSTREE_H
