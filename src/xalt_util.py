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
from __future__         import print_function
import logging
from   logging.handlers import SysLogHandler
from   fnmatch          import fnmatch
import os, re, subprocess

colonPairPat = re.compile(r"([^:]+):(.*)")
def config_logger():
  """
  Configure the logging functions
  """
  logger = logging.getLogger()
  logger.setLevel(logging.INFO)
  try: 
    syslog = SysLogHandler(address='/dev/log', facility='local3')
    formatter = logging.Formatter('XALT: %(name)s: %(levelname)s %(message)r')
    syslog.setFormatter(formatter)
    logger.addHandler(syslog)
  except:
    logger.addHandler(logging.FileHandler("/dev/null"))
    pass
    
  return logger
  
def extract_compiler():
  """
  Take the output of psutil (if available) or pstree and find the compiler
  """
  result = "unknown"
  try:
    from psutil import Process
    p = Process(pid=int(os.getpid()))
    ignore_programs = ['ld', 'collect2','bash','Python']

    def p_parent():
      """
      Determine parent of Process instance.
      Check whether parent is a function (psutil > v2) or a property (psutil < v2)
      """
      return (callable(p.parent) and p.parent()) or p.parent

    def p_parent_name():
      """
      Determine the name of a parent of Process instance.
      Check whether parent is a function (psutil > v2) or a property (psutil < v2)
      """
      return (callable(p.parent) and p.parent().name()) or p.parent.name()

    while p_parent():
      if p_parent_name() not in ignore_programs:
        result = p_parent_name()
        break
      p=p_parent()
  except ImportError:
    ignore_programs = ['pstree', 'ld', 'collect2', 'python', 'sh']
    pstree_bin = "@path_to_pstree@"
    pstree = capture("%s -l -s %d" % (pstree_bin, os.getpid())).strip()
    if (pstree == "unknown"):
      return result

    a = pstree.split("---")

    for cmd in reversed(a):
      if cmd not in ignore_programs:
        result = cmd
        break

  return result

def files_in_tree(path, pattern):
  """
  Find a list of files in directory [[path]] that match [[pattern]]
  @param path: directory.
  @param pattern: file glob pattern.
  """
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
  """
  Find full path to [[program]]
  @param program: program to find in path.
  """

  def is_exe(fpath):
    """
    Check to see if [[fpath]] exists and is executable
    @param fpath: full path to executable
    """
    return os.path.exists(fpath) and os.access(fpath, os.X_OK)

  if (not program):
    return None

  fpath, fname = os.path.split(program)
  if (fpath):
    if (is_exe(program)):
       return os.path.realpath(program)
  else:
    for path in os.environ.get("PATH","").split(os.pathsep):
      exe_file = os.path.join(path, program)
      if (is_exe(exe_file)):
        return os.path.realpath(exe_file)

  return None

def capture(cmd):
  """
  Capture standard out for a command.
  @param cmd: Command string or array.
  """

  if (type(cmd) == type(' ')):
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT, shell =True)
  else:
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT)

    
  return p.communicate()[0]


def carp(key, v):
  """
  Check to make sure that the first part of v contains key.  Raise exception if not.
  
  @param key: key
  @param v: value
  """

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
  """
  Remove this list of files.  Do not raise exception if file doesn't exist.
  @param fileA: array of file
  """

  for f in fileA:
    try:
      os.remove(f)
    except:
      pass
    
numberPat = re.compile(r'[0-9][0-9]*')
def obj_type(object_path):
  """
  Determine what object type [[object_path]] is (a, o, so)
  @param object_path: name of object.
  """
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
  """
  Find module for [[object_path]] in [[reverseMapT]] if it is exists, "NULL" otherwise.
  @param object_path: full object path
  @param reverseMapT: reverse map table. Paths to modules
  """


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


def dbConfigFn(dbname):
  """
  Build config file name from dbname.
  @param dbname: db name
  """
  return dbname + "_db.conf"

