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
from __future__         import print_function
import logging
from   logging.handlers import SysLogHandler
from   fnmatch          import fnmatch
import os, re, sys, subprocess, getent

colonPairPat = re.compile(r"([^:]+):(.*)")
def config_logger():
  logger = logging.getLogger()
  logger.setLevel(logging.INFO)
  syslog = SysLogHandler(address='/dev/log', facility='local3')
  formatter = logging.Formatter('XALT: %(name)s: %(levelname)s %(message)r')
  syslog.setFormatter(formatter)
  logger.addHandler(syslog)
  return logger
  
def extract_compiler(pstree):
  result  = "unknown"
  ignoreT = {
    'pstree'   : True,
    'ld'       : True,
    'collect2' : True,
    }
    
  if (pstree == "unknown"):
    return result

  a = pstree.split("---")
  n = len(a)

  for cmd in reversed(a):
    if (not (cmd in ignoreT)):
      result = cmd
      break

  return cmd

def files_in_tree(path, pattern):
  fileA = []
  wd = os.getcwd()
  if (not os.path.isdir(path)):
    return fileA

  os.chdir(path)
  path = os.getcwd()
  os.chdir(wd)

  for root, dirs, files in os.walk(path):
    for name in files:
      fn = os.path.join(root, name)
      if (fnmatch(fn,pattern)):
        fileA.append(fn)
  return fileA  

def which(program):
  def is_exe(fpath):
    return os.path.exists(fpath) and os.access(fpath, os.X_OK)
  def ext_candidates(fpath):
    yield fpath
    for ext in os.environ.get("PATH", "").split(os.pathsep):
      yield fpath + ext

  if (not program):
    return None
  

  fpath, fname = os.path.split(program)
  if fpath:
    if is_exe(program):
       return os.path.realpath(program)
  else:
    for path in os.environ.get("PATH","").split(os.pathsep):
      exe_file = os.path.join(path, program)
      for candidate in ext_candidates(exe_file):
        if is_exe(candidate):
          return os.path.realpath(candidate)

  return None

def capture(cmd):
  if (type(cmd) == type(' ')):
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT, shell =True)
  else:
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT)

    
  return p.communicate()[0]


def carp(key, v):
  k     = "unknown"
  m     = colonPairPat.match(v)
  found = False
  if (m):
    k = m.group(1)
    if (k == key):
      found = True
  if (not found):
    print("Wanted: ",key, ", got: ",v)
    raise Exception
        
def remove_files(fileA):
  for f in fileA:
    try:
      os.remove(f)
    except:
      pass
    
def passwd_generator():
  xaltUserA = os.environ.get("XALT_USERS")
  if (xaltUserA):
    for user in xaltUserA.split(":"):
      yield user, os.path.expanduser("~" + user)

  else:
    for entry in getent.passwd():
      yield entry.name, entry.dir

numberPat = re.compile(r'[0-9][0-9]*')
def obj_type(object_path):
  result = None
  a      = object_path.split('.')
  for entry in reversed(a):
    m = numberPat.search(entry)
    if (m):
      continue
    else:
      result = entry
      break
  return result

defaultPat = re.compile(r'default:?')
def obj2module(object_path, reverseMapT):
  dirNm, fn  = os.path.split(object_path)
  moduleName = 'NULL'
  pkg         = reverseMapT.get(dirNm)
  if (pkg):
    flavor    = pkg['flavor'][0]
    flavor    = defaultPat.sub('',flavor)
    if (flavor):
      moduleName = "'" + pkg['pkg'] + '(' + flavor + ")'"
    else:
      moduleName = "'" + pkg['pkg'] + "'"
  return moduleName

