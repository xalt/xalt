#!/usr/bin/env python
# -*- python -*-
from __future__  import print_function
import os, sys, re, json, socket, subprocess

parenPat  = re.compile(r'.*\((.*)\).*')
tmpObjPat = re.compile(r'/tmp/[_a-zA-Z0-9-]+.o')

def cleanup(xaltobj, fn):
  f     = open(fn,"r")
  lines = f.readlines()
  d     = {}
  for s in lines:

    # remove lines with ':'
    if (s.find(":")     != -1):
      continue

    # remove line with the xalt.o file
    if (s.find(xaltobj) != -1):
      continue

    # remove a file that is something like: /tmp/ccT33qQt.o
    m = tmpObjPat.search(s)
    if (m):
      continue

    # Capture the library name in the parens:
    # -lgcc_s (/usr/lib/gcc/x86_64-linux-gnu/4.8/libgcc_s.so)
    m = parenPat.search(s)
    if (m):
      d[m.group(1)] = True
      continue

  
    # Save everything else
    idx = s.find("\n")
    if (idx != -1):
      s = s[:idx]

    d[s] = True

  # make the list unique  
  sA = d.keys()

  return sA
    
def capture(cmd):
  p = subprocess.Popen(cmd, stdout=subprocess.PIPE,  stderr=subprocess.STDOUT)
  return p.communicate()[0]
  
def main():
  uuid        = sys.argv[1]
  status      = sys.argv[2]
  wd          = sys.argv[3]
  execname    = sys.argv[4]
  xaltobj     = sys.argv[5]
  build_epoch = sys.argv[6]
  linklineFn  = sys.argv[7]
  resultFn    = sys.argv[8]
  
  

  # Step one clean up linkline data
  sA = cleanup(xaltobj, linklineFn)
  
  resultT                = {}
  resultT['link_id']     = uuid
  resultT['build_user']  = os.environ['USER']
  resultT['exit_code']   = status
  resultT['build_epoch'] = build_epoch
  resultT['exec_name']  = execname
  resultT['hash_id']     = capture(['sha1sum', execname]).split()[0]
  resultT['wd']          = wd
  resultT['build_host']  = socket.getfqdn()
  resultT['linkA']       = sA
  
  dirname,fn = os.path.split(resultFn)

  tmpFn      = os.path.join(dirname, "." + fn)

  try:
    if (not os.path.isdir(dirname)):
      os.mkdir(dirname);
    
    s = json.dumps(resultT, sort_keys=True, indent=2, separators=(',',': '))



    f = open(tmpFn,'w')
    f.write(s)
    f.close()
    os.rename(tmpFn, resultFn)
  except (OSError):
    pass


if ( __name__ == '__main__'): main()
