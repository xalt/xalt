from __future__ import print_function
import os, re, sys
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))

from xalt_util  import which


def find_cmd(ignoreT, i, argA):
  N   = len(argA)
  cmd = None
  while (i < N):
    arg = argA[i]
    i   = i + 1
    bare = os.path.basename(arg)
    if (not (bare in ignoreT)):
      cmd = arg
      break
  return cmd

def find_exec(ignoreT, argT, cmdArg, argA, *n, **kw):
  N   = len(argA)

  if ('dot' in kw):
    os.environ['PATH'] = os.environ['PATH'] + ":."

  i   = 0
  while (i < N):
    arg = argA[i]
    if (arg == cmdArg):
      return which(find_cmd(ignoreT, 0, argA[i+1].split()))
    
    n   = argT.get(arg,-1)
    if (n > 0):
      i += n + 1
      continue
    if (arg[0:1] == "-"):
      i  = i + 1
      continue
    break

  path = which(find_cmd(ignoreT, i, argA)) or "unknown"
  return path


  
