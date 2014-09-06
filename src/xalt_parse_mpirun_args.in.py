#-----------------------------------------------------------------------
# XALT: A tool to track the programs on a cluster.
# Copyright (C) 2013-2014 Robert McLay and Mark Fahey
# 
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of 
# the License, or (at your option) any later version. 
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser  General Public License for more details. 
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA
#-----------------------------------------------------------------------

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


  
