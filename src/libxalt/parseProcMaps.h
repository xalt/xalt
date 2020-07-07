#ifndef PARSE_PROC_MAPS_H
#define PARSE_PROC_MAPS_H

#include <sys/types.h>
#include <unistd.h>
#include "xalt_types.h"

void parseProcMaps(pid_t my_pid, SET_t** libT);

#endif //PARSE_PROC_MAPS_H
