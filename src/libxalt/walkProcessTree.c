#include "process.h"
#include "processTree.h"
#include "utlist.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "xalt_c_utils.h"

void walkProcessTree(pid_t ppid, processTree_t** ptA)
{
  pid_t          my_pid = ppid;
  processTree_t* entry;

  process_t proc;

  HERE;
  init_proc(&proc);
  HERE;

  while(1)
    {
      build_proc(&proc, my_pid);
      if (proc.m_parent < 2)
        break;

  HERE;
      entry = (processTree_t *) malloc(sizeof(processTree_t));
      if (! entry )
        exit(-1);

  HERE;
      utstring_new(entry->m_path);
      utstring_bincpy(entry->m_path, utstring_body(proc.m_exe), utstring_len(proc.m_exe));

  HERE;
      utstring_new(entry->m_name);
      utstring_bincpy(entry->m_name, utstring_body(proc.m_name), utstring_len(proc.m_name));
      
  HERE;
      utarray_new(entry->m_cmdlineA, &ut_str_icd);
      proc_cmdline(&proc, &entry->m_cmdlineA);
      entry->m_pid = proc.m_pid;

  HERE;
      DL_APPEND(*ptA, entry);
      my_pid = proc.m_parent;
  HERE;
    }

  HERE;
  free_proc(&proc);
}

void processTreeFree(processTree_t** ptA)
{
  processTree_t* entry, *tmp;

  DL_FOREACH_SAFE(*ptA, entry, tmp)
    {
      DL_DELETE(*ptA, entry);
      utstring_free(entry->m_path);
      utstring_free(entry->m_name);
      utarray_free(entry->m_cmdlineA);
      my_free(entry, sizeof(processTree_t));
    }
  ptA = NULL;
}
