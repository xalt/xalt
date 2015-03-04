#-----------------------------------------------------------------------
# XALT: A tool that tracks users jobs and environments on a cluster.
# Copyright (C) 2013-2014 University of Texas at Austin
# Copyright (C) 2013-2014 University of Tennessee
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
import os, re, sys, json
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))

from xalt_util  import which


def find_cmd(ignoreT, i, argA):
  """
  Walk the command line and find the command by ignoring names in ignoreT
  such as env time, etc

  @param ignoreT: Table of names to ignore.
  @param i:       The location in the argA array to start looking.
  @param argA:    The command line split into an array.
  """
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

def default_compute_ntasks(t):
  tasks   = int(t.get("tasks",1))
  threads = int(t.get("threads",1))
  return tasks*threads

def find_exec(ignoreT, argT, npT, cmdArg, argA, *n, **kw):
  """
  Walk the command line and first walk over the command line options.

  @param ignoreT: Table of names to ignore.
  @param argT:    Table of command line args. Keys are the name of the argument, values are the number of arguments it takes (> 0).
  @param npT:     Options for the number of tasks.
  @param cmdArg:  command (like in ibrun.symm) that points to a user program.
  @param argA:    The command line split into an array.
  @param kw:      If dot=True then add "." to the end of PATH.
  """
  N   = len(argA)

  compute_ntasks=kw.get("compute_ntasks",default_compute_ntasks)

  if ('dot' in kw):
    os.environ['PATH'] = os.environ.get('PATH',"") + ":."

  i    = 0
  done = False
  resultA = []

  path = None
  while (not done):
    t = {}
    while (i < N):
      arg = argA[i]

      i = parse_ntasks(npT, arg, i, argA, t)

      if (arg == cmdArg):
        path = which(find_cmd(ignoreT, 0, argA[i+1].split()))
        break
      
      n   = argT.get(arg,-1)
      if (n > 0):
        i += n + 1
        continue
      if (arg[0:1] == "-"):
        i  = i + 1
        continue
      break
    if (not path):
      path    = which(find_cmd(ignoreT, i, argA)) or "unknown"
    ntasks  = compute_ntasks(t)
    resultA.append({'exec_prog':path, 'ntasks':ntasks})

    # Loop for colons
    done = True
    while (i < N):
      arg = argA[i]
      i   = i + 1
      if (arg == ":"):
        done = False
        path = None
        break
      
  return json.dumps(resultA)

patSingleOpt = re.compile(r'(-[a-zA-Z])(\d*)')
patLongOpt   = re.compile(r'(--[a-zA-Z]+)=(\d+)')

def parse_ntasks(npT, arg, i, argA, t):
  """
  Parse a single option for task, threads and nodes.
  @param npT:     Options for the number of tasks.
  @param arg:     The single argument.
  @param i:       the current index into argA.
  @param argA:    The array of command line arguments.
  @param t:       result table.
  """
  
  opt   = arg
  value = None

  # Search for pattern -n124 or -n
  m = patSingleOpt.match(arg)
  if (m):
    opt = m.group(1)
    if (m.group(2) != ""):
      value = m.group(2)

  # Search for pattern --nodes=123
  m = patLongOpt.match(arg)
  if (m):
    opt   = m.group(1)
    value = m.group(2)

  if ( opt in npT):
    key = npT[opt]
    if (value == None):
      i = i + 1
      value = argA[i]
    t[key] = value

  return i
