import logging
from   fnmatch          import fnmatch
from   logging.handlers import SysLogHandler
import os, subprocess

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
    for path in os.environ["PATH"].split(os.pathsep):
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
