import os, subprocess

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
       return os.path.realpath(program)
  else:
    for path in os.environ["PATH"].split(os.pathsep):
      exe_file = os.path.join(path, program)
      for candidate in ext_candidates(exe_file):
        if is_exe(candidate):
          return os.path.realpath(candidate)

  return None

def capture(cmd):
  p = subprocess.Popen(cmd, stdout=subprocess.PIPE,  stderr=subprocess.STDOUT)
  return p.communicate()[0]
  
