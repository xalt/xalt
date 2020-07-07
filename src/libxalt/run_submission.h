#ifndef RUN_SUBMISSION_H
#define RUN_SUBMISSION_H

#include <sys/types.h>
#include <unistd.h>

void run_submission(pid_t pid, pid_t ppid, double start_time, double end_time, double probability,
                    char* exec_pathQ, long my_size, int num_gpus, int xalt_type, 
                    const char* uuid_str, const char* watermark, const char* usr_cmdline,
                    char* env[]);


#endif //RUN_SUBMISSION_H
