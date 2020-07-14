#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include "utstring.h"
#include "utarray.h"
#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif


  typedef struct {
    UT_string* m_exe;
    UT_string* m_name;
    pid_t      m_pid;
    pid_t      m_parent;
  } process_t;

  void init_proc(    process_t* proc);
  void build_proc(   process_t* proc, pid_t my_pid);
  void proc_cmdline( process_t* proc, UT_array** cmdlineA);
  void free_proc(    process_t* proc);

#ifdef __cplusplus
}
#endif
#endif //PROCESS_H
