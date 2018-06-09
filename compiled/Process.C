#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "xalt_fgets_alloc.h"
#include "Process.h"
Process::Process(pid_t pid)
{

  //**************************************************
  // Open and read /proc/<pid>/stat
  // pid(%d) exe(%s) state(%c) ppid(%d)

  m_pid = pid;
  char fn[40];
  sprintf(fn,"/proc/%d/stat",m_pid);
  FILE* fp    = fopen(fn,"r");
  if (fp == NULL)
    {
      m_name   = "";
      m_parent = -1;
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
  m_name.assign(start, n);

  m_parent = (int) strtol(p+3, (char **) NULL, 10);
  free(buf); sz = 0; buf = NULL;
}

void Process::cmdline(std::vector<std::string>& cmdlineA)
{
  char fn[40];

  //**************************************************
  // read command line from /proc/<pid>/cmdline
  sprintf(fn,"/proc/%d/cmdline",m_pid);
  FILE* fp    = fopen(fn,"r");
  
  if (fp == NULL)
    {
      cmdlineA.push_back("");
      return;
    }

  char*  buf  = NULL;
  size_t sz   = 0;
  xalt_fgets_alloc(fp, &buf, &sz);
  fclose(fp);

  char * p = buf;
  while (*p)
    {
      cmdlineA.push_back(p);
      p += strlen(p) + 1;
    }

  free(buf); sz = 0; buf = NULL;
}

std::string& Process::exe()
{  
  const size_t bufSz = 2049;
  char fn[40];
  char buf[bufSz];

  //**************************************************
  //Use readlink to get full path to executable.
  sprintf(fn,"/proc/%d/exe",m_pid);

  ssize_t  r   = readlink(fn, buf, bufSz);
  if (r < 0)
    m_exe = "";
  else
    {
      buf[r] = '\0';
      m_exe  = buf;
    }
  return m_exe;
}
    
