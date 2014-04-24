#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function

import os, re, sys, subprocess, time, socket, json, argparse, platform

def capture(cmd):
  p = subprocess.Popen(cmd, stdout=subprocess.PIPE,  stderr=subprocess.STDOUT)
  return p.communicate()[0]
  

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
    host   = syshost()
    parser = argparse.ArgumentParser()
    parser.add_argument("--start",   dest='startTime', action="store", type=float, default="0.0", help="start time")
    parser.add_argument("--fn",      dest='resultFn',  action="store", default = "/dev/null",     help="resultFn")
    parser.add_argument("--ntasks",  dest='ntasks',    action="store", default = "1",             help="number of mpi tasks")
    parser.add_argument("--host",    dest='host',      action="store", default = host,            help="system host name")
    parser.add_argument("exec_name", nargs='+',        help="user program")

    args = parser.parse_args()
    
    return args
    
#class SystemT(object):
#  def __init__(self):
#    sysT = {}
#
#    queueType = "SLURM"
#    if (os.environ.get("SGE_ACCOUNT")):
#      queueType = "SGE"
#    elif (os.environ.get("SLURM_TACC_ACCOUNT")):
#      queueType = "SLURM"
#      
#    if (queueType == "SGE"):
#      sysT['num_cores'] = "NSLOTS"
#      sysT['num_nodes'] = "NHOSTS"
#      sysT['account']   = "SGE_ACCOUNT"
#      sysT['job_id']    = "JOB_ID"
#      sysT['queue']     = "QUEUE"
#      
#    elif (queueType == "SLURM"):
#      sysT['num_cores'] = "SLURM_TACC_CORES"
#      sysT['num_nodes'] = "SLURM_NNODES"
#      sysT['account']   = "SLURM_TACC_ACCOUNT"
#      sysT['job_id']    = "SLURM_JOB_ID"
#      sysT['queue']     = "SLURM_QUEUE"
#
#    self.__sysT = sysT
#      
#  def sysT(self):
#    return self.__sysT

keyPat = re.compile(r'.*<(.*)>.*')

class ExtractXALT(object):

  def __init__(self, cmd):
    self.__fieldT = self.__build_fieldT(cmd)

  def xaltRecordT(self):
    return self.__fieldT

  def __build_fieldT(self, cmd):
    outputA = capture(["objdump", "-s", "-j", ".xalt", cmd]).split('\n')
    fieldT = {}


    if (not outputA[3].find("Contents of section .xalt:") != -1):
      return fieldT
    
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
  def __init__(self, startT, endT, args, userExec):


    ltime                 = time.time()
    userT                 = {}
    userT['nodehost']     = args.system_name
    userT['num_threads']  = os.environ.get("OMP_NUM_THREADS","0")
    userT['user']         = os.environ.get("USER","unknown")
    userT['num_tasks']    = args.ntasks
    userT['start_date']   = time.strftime("%c",time.localtime(startT))
    userT['start_time']   = startT
    userT['currentEpoch'] = ltime
    userT['end_time']     = endT
    userT['run_time']     = max(0, endT - startT)
    userT['exec']         = userExec.execName()
    userT['exec_type']    = userExec.execType()
    userT['exec_epoch']   = userExec.execEpoch()
    userT['execModify']   = userExec.execModify()

    self.__userT = userT
    
    #userT['num_cores']    = os.environ.get(sysT['num_cores'],"0")
    #userT['num_nodes']    = os.environ.get(sysT['num_nodes'],"0")
    #userT['account']      = os.environ.get(sysT['account'],"unknown")
    #userT['job_id']       = os.environ.get(sysT['job_id'],"unknown")
    #userT['queue']        = os.environ.get(sysT['queue'],"unknown")

  def userT(self):
    return self.__userT

class UserExec(object):
  
  def __init__(self, execname):
    self.__execName = execname
    ldd             = capture(["ldd", self.__execName])
    
    self.__execType = None
    if (ldd.find("not a dynamic executable") > 0):
      self.__execType = "script"
    if (ldd.find("No such file or directory") == -1):
      self.__execType = "binary"

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
      re.compile(r'^_.*$'),
    ]
    keepT = {
      '_LMFILES_' : True,
    }


    envT = {}
    for k in os.environ:

      if (k in keepT):
        envT[k] = os.environ[k]
        continue

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

  myEpoch  = time.time() 
  epochStr = "%.5f" % myEpoch

  # parse command line options:
  args = CmdLineOptions().execute()
  
  if (args.startTime < 1):
    startTime = myEpoch
    endTime   = 0
  else:
    startTime = args.startTime
    endTime   = myEpoch
    

  userExec = args.exec_name
  userT    = UserEnvT(startTime, endTime, args, userExec).userT()
  
  submitT              = {}
  submitT['userT']     = userT
  submitT['xaltLinkT'] = ExtractXALT(userExec.execName()).xaltRecordT()
  submitT['libA']      = userExec.libA()
  submitT['envT']      = EnvT().envT()
  submitT['hash']      = userExec.hash()
  
  dirname,fn = os.path.split(os.path.abspath(args.resultFn))
  tmpFn      = os.path.join(dirname, "." + fn)

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

  if (args.startTime < 1):
    print(epochStr)

if ( __name__ == '__main__'): main()
