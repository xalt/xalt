#!/bin/sh
# -*- python -*-

################################################################################
# This file is python 2/3 bilingual. 
# The line """:" starts a comment in python and is a no-op in shell.
""":"
# Shell code to find and run a suitable python interpreter.
for cmd in python3 python python2; do
   command -v > /dev/null $cmd && exec $cmd $0 "$@"
done

echo "Error: Could not find a valid python interpreter --> exiting!" >&2
exit 2
":""" # this line ends the python comment and is a no-op in shell.
################################################################################

# Git Version: @git@

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

#  xalt_json_to_db takes the output found in the ~/.xalt.d/[link,run]*
#  output files and puts it into the database
#
#  optional input:
#    XALT_USERS:  colon separated list of users; only these users are 
#       considered instead of all
#

from __future__  import print_function
import os, sys, re, MySQLdb, json, time, argparse, time, traceback

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from Rmap_XALT     import Rmap
from XALTdb        import XALTdb
from XALTdb        import TimeRecord
from ctypes        import *
from progressBar   import ProgressBar
from xalt_util     import *
from xalt_global   import *
import warnings, getent
warnings.filterwarnings("ignore", "Unknown table.*")

logger       = config_logger()

my_epoch     = time.time()

import inspect

def __LINE__():
    try:
        raise Exception
    except:
        return sys.exc_info()[2].tb_frame.f_back.f_lineno

def __FILE__():
    return inspect.currentframe().f_code.co_filename

#print ("file: '%s', line: %d" % (__FILE__(), __LINE__()), file=sys.stderr)

#libcrc = CDLL(os.path.realpath(os.path.join(dirNm, "../lib64/libcrcFast.so")))

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--delete",      dest='delete',  action="store_true", help="delete files after reading")
    parser.add_argument("-D",            dest='debug',   action="store_true", help="Debug Flag")
    parser.add_argument("--testing",     dest='testing', action="store_true", help="Testing Flag")
    parser.add_argument("--timer",       dest='timer',   action="store_true", help="Time runtime")
    parser.add_argument("--report_file", dest='listFn',  action="store_true", help="list file")
    parser.add_argument("--reverseMapD", dest='rmapD',   action="store",      help="Path to the directory containing the json reverseMap")
    parser.add_argument("--u2acct",      dest='u2acct',  action="store",      help="Path to the json file containing default charge account strings for users")
    parser.add_argument("--syshost",     dest='syshost', action="store",      default="*",            help="name of the cluster")
    parser.add_argument("--confFn",      dest='confFn',  action="store",      default="xalt_db.conf", help="Name of the database")
    args = parser.parse_args()
    return args


def Version():
  my_version = "@git@"
  return my_version


def keep_or_delete(fn, deleteFlg):
  delta = my_epoch - os.stat(fn).st_mtime
  if (delta > 86400 and deleteFlg):
    os.remove(fn)

def check_string_w_crc(s, crcStr, debug):
  if (not crcStr):
    return True

  crcV  = int(crcStr, 16)
  ss     = '{"crc":"0xFFFF"' + s[15:]
  myLen = len(ss)
  data  = ss.encode()
  c     = libcrc.crcFast(c_char_p(data), myLen) & 0xFFFF
  iret  = crcV == c
  if ((not iret) and debug):
    print("  --> crcStr:",crcStr.lower()," computed crc:",hex(c))
    
  return iret


def link_json_to_db(xalt, debug, listFn, reverseMapT, deleteFlg, linkFnA, countT, active, pbar):
  """
  Reads in each link file name and converts json to python table and sends it to be written to DB.

  @param xalt:        An XALTdb object.
  @param debug:       A flag that prints status of each record.
  @param listFn:      A flag that causes the name of the file to be written to stdout.
  @param reverseMapT: The Reverse Map Table.
  @param deleteFlg:   A flag that says to delete files after processing.
  @param linkFnA:     An array of link file names

  """
  num    = 0
  query  = ""

  try:
    for fn in linkFnA:
      XALT_Stack.push("fn: "+fn)   # push fn

      try:
        f     = open(fn,"r")
      except:
        sys.stdout.write(fn+"\n")
        sys.stdout.write("  --> failed to record: Unable to open\n")
        continue
  
      if (listFn or debug):
        sys.stdout.write(fn+"\n")
        if (debug): sys.stdout.write("  --> Trying to open file\n")
        if (debug): sys.stdout.write("  --> Trying to load json\n")
      s = None
      try:
        s     = f.read().rstrip()
        linkT = json.loads(s)
      except:  
        f.close()
        v = XALT_Stack.pop()
        if (debug):
          sys.stdout.write("  --> failed to record: Bad json\n")
        else:
          keep_or_delete(fn, deleteFlg)
        continue

      f.close()

      #if (debug):   sys.stdout.write("  --> Checking CRC\n")
      #if (not check_string_w_crc(s, linkT.get('crc'), debug) ):
      #  if (debug): sys.stdout.write("  --> failed to record: CRC did not match\n")
      #  continue;

      if (debug):
        sys.stdout.write("  --> Sending record to xalt.link_to_db()\n")
      xalt.link_to_db(debug, reverseMapT, linkT)
      num  += 1
      if (active):
        countT['any'] += 1
        if (not debug): pbar.update(countT['any'])

      try:
        if (deleteFlg):
          os.remove(fn)
      except:
        pass

      v     = XALT_Stack.pop()
      carp("fn",v)

  except Exception as e:
    print(XALT_Stack.contents())
    print(query)
    print ("link_json_to_db(): Error: ",e)
    print(traceback.format_exc())
    sys.exit (1)
  return num

def pkg_json_to_db(xalt, debug, listFn, syshost, deleteFlg, pkgFnA, countT, active, pbar):
  """
  Reads in each link file name and converts json to python table and sends it to be written to DB.

  @param xalt:        An XALTdb object.
  @param debug:       A flag that prints status of each record.
  @param listFn:      A flag that causes the name of the file to be written to stderr.
  @param syshost:     The name of the cluster being processed.
  @param deleteFlg:   A flag that says to delete files after processing.
  @param pkgFnA:      An array of link file names

  """
  num = 0
  query = ""

  try:
    for fn in pkgFnA:
      if (listFn or debug):
        sys.stderr.write(fn+"\n")
      XALT_Stack.push("fn: "+fn)   # push fn

      try:
        f     = open(fn,"r")
      except:
        continue
  
      s = None
      try:
        s    = f.read().rstrip()
        pkgT = json.loads(s)
      except:  
        f.close()
        v = XALT_Stack.pop()
        if (debug):
          sys.stdout.write("  --> failed to record: Bad json\n")
        else:
          keep_or_delete(fn, deleteFlg)
        continue

      f.close()
      #if (not check_string_w_crc(s, pkgT.get('crc'), debug) ):
      #  continue;

      xalt.pkg_to_db(debug, syshost, pkgT)
      num  += 1
      if (active):
        countT['any'] += 1
        if (not debug): pbar.update(countT['any'])
      try:
        if (deleteFlg):
          os.remove(fn)
      except:
        pass

      v     = XALT_Stack.pop()
      carp("fn",v)

  except Exception as e:
    print(XALT_Stack.contents())
    print(query)
    print ("pkg_json_to_db(): Error: ",e)
    print(traceback.format_exc())
    sys.exit (1)
  return num


def run_json_to_db(xalt, debug, listFn, reverseMapT, u2acctT, deleteFlg, runFnA, countT, active, pbar, timeRecord):
  """
  Reads in each run file name and converts json to python table and sends it to be written to DB.

  @param xalt:        An XALTdb object.
  @param debug:       A flag that prints status of each record.
  @param listFn:      A flag that causes the name of the file to be written to stdout.
  @param reverseMapT: The Reverse Map Table.
  @param u2acctT:     The map for user to default account string
  @param deleteFlg:   A flag that says to delete files after processing.
  @param runFnA:      An array of run file names

  """
  dupCnt = 0
  num    = 0
  query  = ""
  try:
    for fn in runFnA:
      XALT_Stack.push("fn: "+fn)
      try:
        f      = open(fn,"r")
      except:
        sys.stdout.write(fn+"\n")
        sys.stdout.write("  --> failed to record: Unable to open\n")
        continue
      
      if (listFn or debug):
        sys.stdout.write(fn+"\n")
        if (debug): sys.stdout.write("  --> Trying to open file\n")
        if (debug): sys.stdout.write("  --> Trying to load json\n")
      s = None
      try:
        s    = f.read().rstrip()
        runT = json.loads(s)
      except:
        f.close()
        v = XALT_Stack.pop()
        if (debug):
          sys.stdout.write("  --> failed to record: Bad json\n")
        else:
          keep_or_delete(fn, deleteFlg)
        continue
      f.close()

      
      #if (debug):   sys.stdout.write("  --> Checking CRC\n")
      #if (not check_string_w_crc(s, runT.get('crc'), debug) ):
      #  if (debug): sys.stdout.write("  --> failed to record: CRC did not match\n")
      #  continue;
      
      if (debug):
        sys.stdout.write("  --> Sending record to xalt.run_to_db()\n")
      stored, dups = xalt.run_to_db(debug, reverseMapT, u2acctT, runT, timeRecord)

      try:
        if (deleteFlg):
          os.remove(fn)
      except:
        pass
      if (active):
        countT['any'] += 1
        if (not debug): pbar.update(countT['any'])
      if (stored):
        num += 1
      if (dups):
        dupCnt += 1
        
      v = XALT_Stack.pop()  
      carp("fn",v)

  except Exception as e:
    print(XALT_Stack.contents())
    print(query.encode("ascii","ignore"))
    print ("run_json_to_db(): Error:",e)
    print(traceback.format_exc())
    sys.exit (1)
  return num, dupCnt

def passwd_generator():
  """
  This generator walks the /etc/passwd file and returns the next
  user and home directory.  If XALT_USERS is set then it used that
  instead.  It is a colon separated list.  

  Super hack: if the colon separated list has a ";" in it then the
  first part is the user the second is the home directory.  This is
  use in testing.
  """

  xaltUserA = os.environ.get("XALT_USERS")
  if (xaltUserA):
    for user in xaltUserA.split(":"):
      idx = user.find(";")
      if (idx != -1):
        hdir = user[idx+1:]
        user = user[:idx]
      else:
        hdir = os.path.expanduser("~" + user)
      yield user, hdir

  else:
    for entry in getent.passwd():
      yield entry.name, entry.dir



def build_resultDir(hdir, transmission, kind):
  tail = ""
  if (transmission == "file_separate_dirs"):
    tail = kind

  
  prefix = os.environ.get("XALT_FILE_PREFIX","@xalt_file_prefix@")
  if (not prefix or prefix == "USE_HOME"):
    return os.path.join(hdir,".xalt.d",tail)

  return os.path.join(prefix,tail)

def store_json_files(username, homeDir, transmission, xalt, rmapT, u2acctT, args, countT, pbar, timeRecord):

  xaltDir = build_resultDir(homeDir, transmission, "")
  if (username and args.debug):
    sys.stdout.write("\nSearching for *.json files for "+username+" in "+xaltDir+"\n")

  active = True
  if (homeDir):
    countT['any'] += 1
    if (not args.debug): pbar.update(countT['any'])
    active = False
  

  linkCnt = 0
  runCnt  = 0
  pkgCnt  = 0
  XALT_Stack.push("Directory: " + xaltDir)

  xaltDir = build_resultDir(homeDir, transmission, "link")
  if (os.path.isdir(xaltDir)):
    XALT_Stack.push("link_json_to_db()")
    if (args.debug): sys.stdout.write("Searching for */link.*.json file in "+xaltDir+"\n")
    linkFnA         = files_in_tree(xaltDir, "*/link." + args.syshost + ".*.json")
    linkFnA.sort()
    linkCnt         = len(linkFnA)
    if (args.debug): sys.stdout.write("  --> Found "+str(linkCnt)+" link.*.json files\n\n")
    countT['lnk']  += link_json_to_db(xalt, args.debug, args.listFn, rmapT, args.delete, linkFnA, countT, active, pbar)
    XALT_Stack.pop()
  XALT_Stack.pop()

  xaltDir = build_resultDir(homeDir, transmission, "run")
  XALT_Stack.push("Directory: " + xaltDir)
  if (os.path.isdir(xaltDir)):
    XALT_Stack.push("run_json_to_db()")
    if (args.debug): sys.stdout.write("Searching for */run.*.json file in "+xaltDir+"\n")
    runFnA         = files_in_tree(xaltDir, "*/run." + args.syshost + ".*.json") 
    runFnA.sort();
    runCnt         = len(runFnA)
    if (args.debug): sys.stdout.write("  --> Found "+str(runCnt)+" run.*.json files\n\n")
    num, dups      = run_json_to_db(xalt, args.debug, args.listFn, rmapT, u2acctT, args.delete, runFnA, 
                                    countT, active, pbar, timeRecord)
    countT['run']  += num
    countT['dup']  += dups
    XALT_Stack.pop()
  XALT_Stack.pop()

  xaltDir = build_resultDir(homeDir, transmission, "pkg")
  XALT_Stack.push("Directory: " + xaltDir)
  if (os.path.isdir(xaltDir)):
    XALT_Stack.push("pkg_json_to_db()")
    if (args.debug): sys.stdout.write("Searching for */pkg.*.json file in "+xaltDir+"\n")
    pkgFnA         = files_in_tree(xaltDir, "*/pkg." + args.syshost + ".*.json") 
    pkgFnA.sort()
    pkgCnt         = len(pkgFnA)
    if (args.debug): sys.stdout.write("  --> Found "+str(pkgCnt)+" pkg.*.json files\n\n")
    countT['pkg'] += pkg_json_to_db(xalt, args.debug, args.listFn, args.syshost, args.delete, pkgFnA,
                                    countT, active, pbar)
    XALT_Stack.pop()
  XALT_Stack.pop()

  sum = linkCnt + runCnt + pkgCnt
  if (args.debug and sum > 0):
    extra = ""
    if (username):
      extra = " for user " + username
    print("Storing the following "+str(sum)+" json files (run: "+str(runCnt)+", link: "+str(linkCnt)+", pkg: "+str(pkgCnt)+")"+extra)

def main():
  """
  Walks the list of users via the passwd_generator and load the
  link and run files.
  """

  # Using home directories or a global location.
  xalt_file_prefix = os.environ.get("XALT_FILE_PREFIX","@xalt_file_prefix@")
  
  xaltUserA        = os.environ.get("XALT_USERS")
  if (xaltUserA):
    xaltUserStr    = ", XALT_USERS: \"" + xaltUserA + "\""
  else:
    xaltUserStr    = ""
    


  # Find transmission style
  transmission = os.environ.get("XALT_TRANSMISSION_STYLE")
  if (not transmission):
    transmission = "@transmission@"

  if (not transmission):
    transmission = "file"
    
  transmission = transmission.lower()
  
  timeRecord   = TimeRecord()

  # Push command line on to XALT_Stack
  sA = []
  sA.append("CommandLine:")
  for v in sys.argv:
    sA.append('"'+v+'"')
  XALT_Stack.push(" ".join(sA))

  args      = CmdLineOptions().execute()
  xalt      = XALTdb(args.confFn)
  extra_txt = ""
  if (args.syshost != "*"):
    extra_txt = " for "+args.syshost

  num  = 0
  szS = ""
  if (xalt_file_prefix == "USE_HOME"):
    num     = int(capture("LD_PRELOAD= getent passwd | wc -l"))
  else:
    xaltDir = build_resultDir("", transmission, "")
    allFnA  = files_in_tree(xaltDir, "*/*." + args.syshost + ".*.json")
    num     = len(allFnA)
    szS     = "\n  Number of json files to process: " + str(num)
  pbar    = ProgressBar(maxVal=num, fd=sys.stdout)

  print ("\n################################################################" )
  print ("XALT Git Version: "+Version()+extra_txt                             )
  print ("  Using XALT_FILE_PREFIX: \""+xalt_file_prefix+"\""+xaltUserStr+szS )
  print ("################################################################\n" )
  if (xalt_file_prefix == "USE_HOME" and not args.testing):
    print ("\n################################################################")
    print (" Note using ~/.xalt.d to store result is fine for testing.")
    print (" BUT do not use in production. There is a race condition")
    print (" Please re-configure xalt to use --with-xaltFilePrefix=...")
    print (" See https://xalt.readthedocs.io/en/latest/020_site_configuration.html for details")
    print ("")
    print (" Add the option --testing to silence this warning")
    print ("################################################################\n")

  icnt   = 0

  t1     = time.time()

  rmapT  = Rmap(args.rmapD).reverseMapT()

  u2acctT = {}
  if (args.u2acct):
    fp     = open(args.u2acct,"r")
    u2acctT = json.loads(fp.read())
    fp.close()

  countT = {}
  countT['lnk'] = 0
  countT['run'] = 0
  countT['dup'] = 0
  countT['pkg'] = 0
  countT['any'] = 0

  if (xalt_file_prefix == "USE_HOME"):
    for user, homeDir in passwd_generator():
      store_json_files(user, homeDir, transmission, xalt, rmapT, u2acctT, args, countT, pbar, timeRecord)
  else:
    store_json_files(None, "", transmission, xalt, rmapT, u2acctT, args, countT, pbar, timeRecord)

  xalt.connect().close()
  pbar.fini()
  t2 = time.time()
  rt = t2 - t1
  if (args.timer):
    print("Time: ", time.strftime("%T", time.gmtime(rt)))

  print("num links: ", countT['lnk'], ", num pkgs: ", countT['pkg'], ", num runs: ", countT['run'],", dups: ",countT['dup'] )
  timeRecord.print()
  

if ( __name__ == '__main__'): main()
