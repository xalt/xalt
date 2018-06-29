#ifndef _WALK_PROCESS_TREE_H
#define _WALK_PROCESS_TREE_H
#include <unistd.h>
#include "xalt_types.h"

void walkProcessTree(pid_t ppid, std::vector<ProcessTree>& ptV);

#endif //_WALK_PROCESS_TREE_H
