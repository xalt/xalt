#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "xalt_fgets_alloc.h"
#include "process.h"
#include "utstring.h"
#include "xalt_base_types.h"

void init_proc(process_t* proc)
{
  utstring_new(proc->m_name);
  utstring_new(proc->m_exe);
  proc->m_pid    = -1;
  proc->m_parent = -1;
}

void build_proc(process_t* proc, pid_t my_pid)
{
  utstring_clear(proc->m_name);
  utstring_clear(proc->m_exe);
  proc->m_pid = my_pid;
  char fn[40];
  sprintf(fn, "/proc/%d/stat",proc->m_pid);
  FILE* fp    = fopen(fn,"r");
  if (fp == NULL)
    {
      proc->m_parent = -1;
      return;
    }
  
  char*  buf  = NULL;
  size_t sz   = 0;
  xalt_fgets_alloc(fp, &buf, &sz);
  fclose(fp);

  //**************************************************
  // extract 2nd (exe) and 4th (ppid) fields from stat
  char* p     = strchr(buf,' ');
  char* start = ++p;
  if (*start == '(') start++;

  p           = strchr(p,' ');
  int n       = p - start - 1;
  if (*p     == ')') --n;
  utstring_bincpy(proc->m_name, start, n);

  proc->m_parent = (int) strtol(p+3, (char **) NULL, 10);
  memset(buf, 0, sz);
  sz = 0;
  buf = NULL;
  free(buf);

  const size_t bufSz = 2049;
  char buf2[bufSz];

  //**************************************************
  //Use readlink to get full path to executable.
  sprintf(fn,"/proc/%d/exe",proc->m_pid);

  ssize_t r = readlink(fn, buf2, bufSz);
  if (r > 1)
    utstring_bincpy(proc->m_exe, buf2, r);
}

void free_proc(process_t *proc)
{
  utstring_free(proc->m_name);
  utstring_free(proc->m_exe);
}

void proc_cmdline(process_t* proc, UT_array** p_cmdlineA)
{
  UT_array* cmdlineA = *p_cmdlineA;
  char fn[40];

  //**************************************************
  // read command line from /proc/<pid>/cmdline
  sprintf(fn,"/proc/%d/cmdline",proc->m_pid);
  FILE* fp    = fopen(fn,"r");
  
  if (fp == NULL)
    return;

  char *buf = NULL;
  size_t sz = 0;
  xalt_fgets_alloc(fp, &buf, &sz);
  fclose(fp);
  
  char * p = buf;
  while (*p)
    {
      utarray_push_back(cmdlineA, &p);
      p += strlen(p) + 1;
    }

  memset(buf, 0, sz);
  free(buf); sz = 0; buf = NULL;
}
