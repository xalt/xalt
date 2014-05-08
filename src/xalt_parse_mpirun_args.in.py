from __future__ import print_function
from util       import which
import os, re, sys

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

def find_exec(ignoreT, argT, cmdArg, argA):
  N   = len(argA)

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

  return which(find_cmd(ignoreT, i, argA))

  
