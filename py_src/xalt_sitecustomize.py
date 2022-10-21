#### this should go into sitecustomize.py ####
import sys, os, subprocess

from xalt_python_pkg_filter import keep_pkg

if sys.version_info[0] >= 3:
  string_types = str,
else:
  string_types = basestring,

class RecorderRTM(object):
  """
  Record all requests to load a module by name.
  """
  __slots__ = ['_cmd','_keepT']


  def __init__(self, uuid, version_str):
    self._cmd     = False
    self._keepT   = {}
    xaltDir       = os.environ.get("XALT_DIR")
    if (xaltDir):
      self._cmd     = "XALT_EXECUTABLE_TRACKING=no " + os.path.join(xaltDir,"libexec/xalt_record_pkg") + \
                      " -u " + uuid + " program python xalt_run_uuid " + uuid + " package_version " + version_str
    
  def __keep(self, fullname, path):
    keepT              = self._keepT
    keep, reason, kind = keep_pkg(fullname, path)
    if (not keep):
      return False

    if (fullname in keepT):
      return False
    keepT[fullname] = True
    return True

  def __report(self, fullname, path):
    if (not self._cmd):
      return
    nonStrCount = 0
    if (not isinstance(fullname,string_types)):
      fullname = "'<unknown>'"
      nonStrCount = nonStrCount+1
    if (not isinstance(path,string_types)):
      path = "'<unknown>'"
      nonStrCount = nonStrCount+1
    if (nonStrCount >= 2):
      return
    os.environ['LD_PRELOAD'] = ""
    cmd = self._cmd + " package_name " + fullname + " package_path " + path
    subprocess.call(cmd, shell=True)


  # Python 3.4+
  def find_spec(self, fullname, path, target=None):

    result = None
    for entry in sys.meta_path:
      cls  = type(entry)
      if (cls.__name__ != "RecorderRTM" ):
        try: 
          result = entry.find_spec(fullname, path, target)
        except:
          pass
        if (result):
          break

    if (not result):
      return result

    path = result.origin

    if (self.__keep(fullname, path)):
      self.__report(fullname, path)

    return result

  # Python 2.3--3.3
  def find_module(self, fullname, path):
    result = None
    path   = None

    for entry in sys.meta_path:
      cls  = type(entry)
      if (cls.__name__ != "RecorderRTM" ):
        result = entry.find_module(fullname, path)
        if (result):
          path = result.origin
          break
  
    for entry in sys.path:
      base = os.path.join(entry,fullname)
      fn   = base + ".py"
      if (os.path.exists(fn)):
        path = fn
        break
      else:
        fn   = os.path.join(base, "__init__.py")
        if (os.path.exists(fn)):
          path = fn
          break

    if (path and  self.__keep(fullname, path)):
      self.__report(fullname, path)

    return result




uuid        = os.environ.get("XALT_RUN_UUID",None)
if (uuid):
  sA = []
  for i in sys.version_info:
    s = str(i)
    if (s == 'final'):
      break
    sA.append(s)
  version_str = '.'.join(sA)

  sys.meta_path.insert(0, RecorderRTM(uuid, version_str))

