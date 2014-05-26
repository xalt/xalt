#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

from __future__ import print_function
from util       import capture, which

import os, re, sys, subprocess, time, socket, json, argparse, platform

def syshost():
  hostA = platform.node().split('.')
  idx = 1 
  if (len(hostA) < 2):
    idx = 0
  return hostA[idx]

class CmdLineOptions(object):
  def __init__(self):
    pass
  
  def execute(self):
    parser = argparse.ArgumentParser()
    parser.add_argument("--start",   dest='startTime', action="store", type=float, default="0.0", help="start time")
    parser.add_argument("--end",     dest='endTime',   action="store", type=float, default="0.0", help="end time")
    parser.add_argument("--fn",      dest='resultFn',  action="store", default = "/dev/null",     help="resultFn")
    parser.add_argument("--ntasks",  dest='ntasks',    action="store", default = "-1",            help="number of mpi tasks")
    parser.add_argument("--syshost", dest='syshost',   action="store", default = syshost(),       help="system host name")
    parser.add_argument("--run_uuid",dest='run_uuid',  action="store", default = None,            help="run uuid")
    parser.add_argument("exec_prog", nargs='+',        help="user program")

    args = parser.parse_args()
    
    return args
    
keyPat = re.compile(r'.*<(.*)>.*')

class ExtractXALT(object):

  def __init__(self, cmd):
    self.__fieldT = self.__build_fieldT(cmd)

  def xaltRecordT(self):
    return self.__fieldT

  def __build_fieldT(self, cmd):
    outStr  = capture(["objdump", "-s", "-j", ".xalt", cmd])

    fieldT = {}
    if (not outStr.find("Contents of section .xalt:") != -1):
      return fieldT
    
    outputA = outStr.split('\n')
    outputA.pop(0)
    outputA.pop(0)
    outputA.pop(0)
    outputA.pop(0)
  
    sA = []
    for line in outputA:
      split = line.split()
      if (len(split) > 0):
        sA.append(split[-1])
    s = "".join(sA)
  
    xaltA   = re.split('%%', s)
  
    fieldT = {}

    N = len(xaltA)
    idx = -1
    while (True):
      idx  = idx + 1
      line = xaltA[idx]
      if (line.find("XALT_Link_Info_End") != -1):
        break
      m = keyPat.search(line)
      if (m):
        key   = m.group(1)
        idx   = idx + 1
        value = xaltA[idx].replace("_%_%_"," ")
        fieldT[key] = value
    
    return fieldT 
   


class UserEnvT(object):
  def __init__(self, args, userExec):


    ltime                 = time.time()
    userT                 = {}
    userT['cwd']          = os.getcwd()
    userT['syshost']      = args.syshost
    userT['run_uuid']     = args.run_uuid
    userT['num_threads']  = int(os.environ.get("OMP_NUM_THREADS","0"))
    userT['user']         = os.environ.get("USER","unknown")
    userT['num_tasks']    = args.ntasks
    userT['start_date']   = time.strftime("%c",time.localtime(args.startTime))
    userT['start_time']   = args.startTime
    userT['currentEpoch'] = ltime
    userT['end_time']     = args.endTime
    userT['run_time']     = max(0, args.endTime - args.startTime)
    userT['exec_path']    = userExec.execName()
    userT['exec_type']    = userExec.execType()
    userT['exec_epoch']   = userExec.execEpoch()
    userT['execModify']   = userExec.execModify()

    self.__userT = userT
    
  def userT(self):
    return self.__userT

class UserExec(object):
  
  def __init__(self, exec_progA):

    ignoreT = {
      'env'              : True,
      'time'             : True,
    }
    cmd = None
    for prog in exec_progA:
      bare = os.path.basename(prog)
      if (not (bare in ignoreT)):
        cmd = prog
        break

    self.__execName = which(cmd)
    ldd = None
    if (self.__execName):
      outStr = capture(["file", self.__execName])
      if (outStr.find("ASCII text") > 0):
        self.__execType = "script"
      elif (outStr.find("executable") > 0):
        self.__execType = "binary"
        ldd             = capture(["ldd", self.__execName])
      else:
        self.__execType = None

      info = os.stat(self.__execName)
      self.__modify = info.st_mtime
      self.__libA   = self.__parseLDD(ldd)
      self.__hash   = self.__computeHash(self.__execName)


  def execName(self):
    return self.__execName

  def execType(self):
    return self.__execType

  def execEpoch(self):
    return self.__modify

  def execModify(self):
    return time.strftime("%c",time.localtime(self.__modify))

  def libA(self):
    return self.__libA

  def hash(self):
    return self.__hash

  def __computeHash(self, cmd):
    fieldA = capture(["sha1sum", cmd]).split()
    return fieldA[0]

  def __parseLDD(self,ldd):
    if (ldd.find("not a dynamic executable") > 0):
      return []

    lineA = ldd.split('\n')

    libA = []
    d    = {}
    for line in lineA:
      fieldA = line.split()
      N      = len(fieldA)
      
      if (N < 1):
        break
      elif (N == 4):
        lib = fieldA[2]
      else:
        lib = fieldA[0]
      lib = os.path.realpath(lib)
      d[lib] = True
    
    libA = d.keys()
    libA = sorted(libA)
    
    libB = []
    for lib in libA:
      hash_line = capture(['sha1sum', lib])
      if (hash_line.find("No such file or directory") != -1):
        v = "unknown"
      else:
        v = hash_line.split()[0]

      libB.append([lib, v])
      

    return libB
    
class EnvT(object):
  def __init__(self):
    self.__envT = self.__reportUserEnv()
  def envT(self):
    return self.__envT
  def __reportUserEnv(self):
    ignoreKeyA = [
      re.compile(r'^LESS_TERMCAP_.*$'),
      re.compile(r'^LS_COLORS$'),
      re.compile(r'^SSH_.*$'),
      re.compile(r'^XDG_.*$'),
      re.compile(r'^PS1$'),
    ]

    envT = {}
    for k in os.environ:
      keep = True
      for pat in ignoreKeyA:
        m = pat.search(k)
        if (m):
          keep = False
          break
      if (keep):
        envT[k] = os.environ[k]
    return envT
  

def main():

  try:
    # parse command line options:
    args = CmdLineOptions().execute()

    userExec = UserExec(args.exec_prog)
    if (not userExec.execName()):
      return

    userT    = UserEnvT(args, userExec).userT()
  
    submitT              = {}
    submitT['userT']     = userT
    submitT['xaltLinkT'] = ExtractXALT(userExec.execName()).xaltRecordT()
    submitT['libA']      = userExec.libA()
    submitT['envT']      = EnvT().envT()
    submitT['hash_id']   = userExec.hash()
  
    dirname,fn = os.path.split(os.path.abspath(args.resultFn))
    tmpFn      = os.path.join(dirname, "." + fn)
  except:
    pass

  try:
    if (not os.path.isdir(dirname)):
      os.mkdir(dirname);
    
    s = json.dumps(submitT, sort_keys=True, indent=2, separators=(',',': '))

    f = open(tmpFn,'w')
    f.write(s)
    f.close()
    os.rename(tmpFn, args.resultFn)
  except (OSError):
    pass


if ( __name__ == '__main__'): main()
