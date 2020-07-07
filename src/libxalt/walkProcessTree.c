#include "process.h"
#include "processTree.h"
#include "utlist.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void walkProcessTree(pid_t ppid, processTree_t** ptA)
{
  pid_t          my_pid = ppid;
  processTree_t* entry;
  processTree_t* tmp;

  process_t proc;

  init_proc(&proc);

  while(1)
    {
      build_proc(&proc, my_pid);
      if (proc.m_parent < 2)
	break;

      entry = (processTree_t *) malloc(sizeof(processTree_t));
      if (! entry )
	exit(-1);

      utstring_new(entry->m_path);
      utstring_bincpy(entry->m_path, utstring_body(proc.m_exe), utstring_len(proc.m_exe));

      utstring_new(entry->m_name);
      utstring_bincpy(entry->m_name, utstring_body(proc.m_name), utstring_len(proc.m_name));
      
      utarray_new(entry->m_cmdlineA, &ut_str_icd);
      proc_cmdline(&proc, &entry->m_cmdlineA);
      entry->m_pid = proc.m_pid;

      DL_APPEND(*ptA, entry);
      my_pid = proc.m_parent;
    }

  free_proc(&proc);
}
