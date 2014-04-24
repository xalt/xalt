from __future__ import print_function
import os, re, sys

def which(program):
  def is_exe(fpath):
    return os.path.exists(fpath) and os.access(fpath, os.X_OK)
  def ext_candidates(fpath):
    yield fpath
    for ext in os.environ.get("PATH", "").split(os.pathsep):
      yield fpath + ext

  fpath, fname = os.path.split(program)
  if fpath:
    if is_exe(program):
       return program
  else:
    for path in os.environ["PATH"].split(os.pathsep):
      exe_file = os.path.join(path, program)
      for candidate in ext_candidates(exe_file):
        if is_exe(candidate):
          return candidate

  return None

def find_exec(ignoreT, argT, argA):
  N   = len(argA)

  i   = 0
  while (i < N):
    arg = argA[i]
    n   = argT.get(arg,-1)
    if (n > 0):
      i += n + 1
      continue
    if (arg[0:1] == "-"):
      i  = i + 1
      continue
    break

  while (i < N):
    arg = argA[i]
    i   = i + 1
    bare = os.path.basename(arg)
    if (not (bare in ignoreT)):
      cmd = arg
      break

  return which(cmd)
  
