#!/usr/bin/env python
# -*- python -*-
#
#  xalt_json_to_db takes the output found in the ~/.xalt.d/[link,run]*
#  output files and puts it into the database
#
#  optional input:
#    XALT_USERS:  colon separated list of users; only these users are 
#       considered instead of all
#
#
# Git Version: @git@

from __future__  import print_function
import os, sys, re, MySQLdb, json, time, argparse

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from xalt_stack    import Stack
from XALTdb        import XALTdb
from xalt_site_pkg import translate
from xalt_util     import *
from progressBar   import ProgressBar
from XALT_Rmap     import Rmap
import warnings, getent
warnings.filterwarnings("ignore", "Unknown table.*")

ConfigBaseNm = "xalt_db"
ConfigFn     = ConfigBaseNm + ".conf"
logger       = config_logger()
patSQ        = re.compile("'")
pstack       = Stack()
colonPairPat = re.compile(r"([^:]+):(.*)")

class CmdLineOptions(object):
  def __init__(self):
    pass
  
  def execute(self):
    parser = argparse.ArgumentParser()
    parser.add_argument("--delete",      dest='delete', action="store_true", help="delete files after reading")
    parser.add_argument("--timer",       dest='timer',  action="store_true", help="Time runtime")
    parser.add_argument("--report_file", dest='listFn', action="store_true", help="list file")
    parser.add_argument("--reverseMapD", dest='rmapD',  action="store",      help="Path to the directory containing the json reverseMap")
    args = parser.parse_args()
    return args


def link_json_to_db(xalt, pstack, listFn, reverseMapT, linkFnA):

  num = 0
  query = ""

  try:
    for fn in linkFnA:
      if (listFn):
        sys.stderr.write(fn+"\n")
      pstack.push("fn: "+fn)   # push fn
      f     = open(fn,"r")
      try:
        linkT = json.loads(f.read())
      except:  
        f.close()
        v = pstack.pop()
        carp("fn",v)
        continue

      f.close()
      xalt.link_to_db(pstack, reverseMapT, linkT)
      num  += 1
      v     = pstack.pop()
      carp("fn",v)

  except Exception as e:
    print(pstack.contents())
    print(query)
    print ("link_json_to_db(): Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)
  return num


def run_json_to_db(xalt, pstack, listFn, reverseMapT, runFnA):
  nameA = [ 'num_cores', 'num_nodes', 'account', 'job_id', 'queue', 'submit_host' ]
  num   = 0
  query = ""
  try:
    for fn in runFnA:
      if (listFn):
        sys.stderr.write(fn+"\n")
      pstack.push("fn: "+fn)
      num   += 1
      f      = open(fn,"r")
      
      try:
        runT   = json.loads(f.read())
      except:
        f.close()
        v = pstack.pop()
        carp("fn",v)
        continue
      f.close()

      xalt.run_to_db(pstack, reverseMapT, runT)
      v = pstack.pop()  
      carp("fn",v)

  except Exception as e:
    print(pstack.contents())
    print(query.encode("ascii","ignore"))
    print ("run_json_to_db(): ",e)
    sys.exit (1)
  return num


def main():
  # Push command line on to pstack
  sA = []
  sA.append("CommandLine:")
  for v in sys.argv:
    sA.append('"'+v+'"')

  args   = CmdLineOptions().execute()
  xalt   = XALTdb(ConfigFn)

  num    = int(capture("getent passwd | wc -l"))
  pbar   = ProgressBar(maxVal=num)
  icnt   = 0

  t1     = time.time()

  rmapT  = Rmap(args.rmapD).reverseMapT()

  iuser  = 0
  lnkCnt = 0
  runCnt = 0

  for user, hdir in passwd_generator():
    pstack.push("User: " + user)
    xaltDir = os.path.join(hdir,".xalt.d")
    if (os.path.isdir(xaltDir)):
      iuser   += 1
      linkFnA  = files_in_tree(xaltDir, "*/link.*.json")
      pstack.push("link_json_to_db()")
      lnkCnt  += link_json_to_db(xalt, pstack, args.listFn, rmapT, linkFnA)
      pstack.pop()
      if (args.delete):
        remove_files(linkFnA)
        #remove_files(files_in_tree(xaltDir, "*/.link.*.json"))

      runFnA   = files_in_tree(xaltDir, "*/run.*.json")
      pstack.push("run_json_to_db()")
      runCnt  += run_json_to_db(args.listFn, xalt, user, rmapT, runFnA)
      pstack.pop()
      if (args.delete):
        remove_files(runFnA)
        #remove_files(files_in_tree(xaltDir, "*/.run.*.json"))
    icnt += 1
    v = pstack.pop()
    carp("User",v)
    pbar.update(icnt)

  xalt.connect().close()
  pbar.fini()
  t2 = time.time()
  rt = t2 - t1
  if (args.timer):
    print("Time: ", time.strftime("%T", time.gmtime(rt)))

  print("num users: ", iuser, ", num links: ", lnkCnt, ", num runs: ", runCnt)

if ( __name__ == '__main__'): main()
