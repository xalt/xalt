from __future__ import print_function
from util       import which
import os, re, sys

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
  
