#!/usr/bin/env python
# -*- python -*-
#
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
import os, sys, re, MySQLdb, json, time, argparse

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from XALTdb        import XALTdb
from xalt_site_pkg import translate
from xalt_util     import *
from xalt_global   import *
from progressBar   import ProgressBar
from XALT_Rmap     import Rmap
import warnings, getent
warnings.filterwarnings("ignore", "Unknown table.*")

logger       = config_logger()

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--delete",      dest='delete', action="store_true", help="delete files after reading")
    parser.add_argument("--timer",       dest='timer',  action="store_true", help="Time runtime")
    parser.add_argument("--report_file", dest='listFn', action="store_true", help="list file")
    parser.add_argument("--reverseMapD", dest='rmapD',  action="store",      help="Path to the directory containing the json reverseMap")
    parser.add_argument("--dbname",      dest='dbname', action="store",      default="xalt", help="Name of the database")
    args = parser.parse_args()
    return args


def link_json_to_db(xalt, listFn, reverseMapT, linkFnA):
  """
  Reads in each link file name and converts json to python table and sends it to be written to DB.

  @param xalt:        An XALTdb object.
  @param listFn:      A flag that causes the name of the file to be written to stderr.
  @param reverseMapT: The Reverse Map Table.
  @param linkFnA:     An array of link file names

  """
  num = 0
  query = ""

  try:
    for fn in linkFnA:
      if (listFn):
        sys.stderr.write(fn+"\n")
      XALT_Stack.push("fn: "+fn)   # push fn
      f     = open(fn,"r")
      try:
        linkT = json.loads(f.read())
      except:  
        f.close()
        v = XALT_Stack.pop()
        carp("fn",v)
        continue

      f.close()
      xalt.link_to_db(reverseMapT, linkT)
      num  += 1
      v     = XALT_Stack.pop()
      carp("fn",v)

  except Exception as e:
    print(XALT_Stack.contents())
    print(query)
    print ("link_json_to_db(): Error: ",e)
    sys.exit (1)
  return num


def run_json_to_db(xalt, listFn, reverseMapT, runFnA):
  """
  Reads in each run file name and converts json to python table and sends it to be written to DB.

  @param xalt:        An XALTdb object.
  @param listFn:      A flag that causes the name of the file to be written to stderr.
  @param reverseMapT: The Reverse Map Table.
  @param runFnA:      An array of run file names

  """
  num   = 0
  query = ""
  try:
    for fn in runFnA:
      if (listFn):
        sys.stderr.write(fn+"\n")
      XALT_Stack.push("fn: "+fn)
      num   += 1
      f      = open(fn,"r")
      
      try:
        runT   = json.loads(f.read())
      except:
        f.close()
        v = XALT_Stack.pop()
        carp("fn",v)
        continue
      f.close()

      xalt.run_to_db(reverseMapT, runT)
      v = XALT_Stack.pop()  
      carp("fn",v)

  except Exception as e:
    print(XALT_Stack.contents())
    print(query.encode("ascii","ignore"))
    print ("run_json_to_db(): Error:",e)
    sys.exit (1)
  return num

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



def main():
  """
  Walks the list of users via the passwd_generator and load the
  link and run files.
  """

  # Push command line on to XALT_Stack
  sA = []
  sA.append("CommandLine:")
  for v in sys.argv:
    sA.append('"'+v+'"')
  XALT_Stack.push(" ".join(sA))

  args   = CmdLineOptions().execute()
  xalt   = XALTdb(dbConfigFn(args.dbname))

  num    = int(capture("getent passwd | wc -l"))
  pbar   = ProgressBar(maxVal=num)
  icnt   = 0

  t1     = time.time()

  rmapT  = Rmap(args.rmapD).reverseMapT()

  iuser  = 0
  lnkCnt = 0
  runCnt = 0

  for user, hdir in passwd_generator():
    XALT_Stack.push("User: " + user)
    xaltDir = os.path.join(hdir,".xalt.d")
    if (os.path.isdir(xaltDir)):
      iuser   += 1
      linkFnA  = files_in_tree(xaltDir, "*/link.*.json")
      XALT_Stack.push("link_json_to_db()")
      lnkCnt  += link_json_to_db(xalt, args.listFn, rmapT, linkFnA)
      XALT_Stack.pop()
      if (args.delete):
        remove_files(linkFnA)
        #remove_files(files_in_tree(xaltDir, "*/.link.*.json"))

      runFnA   = files_in_tree(xaltDir, "*/run.*.json")
      XALT_Stack.push("run_json_to_db()")
      runCnt  += run_json_to_db(xalt, args.listFn, rmapT, runFnA)
      XALT_Stack.pop()
      if (args.delete):
        remove_files(runFnA)
        #remove_files(files_in_tree(xaltDir, "*/.run.*.json"))
    icnt += 1
    v = XALT_Stack.pop()
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
