#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

#-----------------------------------------------------------------------
# XALT: A tool to track the programs on a cluster.
# Copyright (C) 2013-2014 Robert McLay and Mark Fahey
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

#  xalt_syslog takes the output found in xalt syslog file
#  provided with the --syslog_file=ans argument and puts it 
#  into the database
#
#  One has to set up syslog to capture the XALT output
#  Example is provided in the xalt_syslog.conf file that can 
#  be put in the /etc/rsyslog.d directory.
#
#

from __future__  import print_function
import os, sys, re, MySQLdb, json, time, argparse, base64

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from XALTdb        import XALTdb
from xalt_site_pkg import translate
from xalt_util     import *
from xalt_global   import *
from progressBar   import ProgressBar
from XALT_Rmap     import Rmap

ConfigBaseNm = "xalt_db"
ConfigFn     = ConfigBaseNm + ".conf"
logger       = config_logger()

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--syslog_file", dest='syslog', action="store",      help="location and name of syslog file")
    parser.add_argument("--timer",       dest='timer',  action="store_true", help="Time runtime")
    parser.add_argument("--reverseMapD", dest='rmapD',  action="store",      help="Path to the directory containing the json reverseMap")
    args = parser.parse_args()
    return args


def main():
  """
  read from syslog file into XALT db.
  """

  sA = []
  sA.append("CommandLine:")
  for v in sys.argv:
    sA.append('"'+v+'"')
  XALT_Stack.push(" ".join(sA))

  args   = CmdLineOptions().execute()
  xalt   = XALTdb(ConfigFn)

  syslogFile  = args.syslog

  # should add a check if file exists
  num    = int(capture("cat "+syslogFile+" | wc -l"))
  icnt   = 0

  t1     = time.time()

  rmapT  = Rmap(args.rmapD).reverseMapT()

  lnkCnt = 0
  runCnt = 0
  badCnt = 0
  count = 0

  if num != 0: 
    pbar   = ProgressBar(maxVal=num)
    if (os.path.isfile(syslogFile)):
      f=open(syslogFile, 'r')
      for line in f:
        if "XALT_LOGGING" in line:

          i=line.find("link:")
          if i == -1 :
            i=line.find("run:")
          if i == -1:
            continue   # did not find a link or run, continue to next line

          array=line[i:].split(":")
          type = array[0].strip()
          syshost = array[1].strip()
          try:
            resultT=json.loads(base64.b64decode(array[2]))
            XALT_Stack.push("XALT_LOGGING: " + type + " " + syshost)
#            XALT_Stack.push("XALT_LOGGING resultT: " + resultT)

            if ( type == "link" ):
              XALT_Stack.push("link_to_db()")
              xalt.link_to_db(rmapT, resultT)
              XALT_Stack.pop()
              lnkCnt += 1
            elif ( type == "run" ):
              XALT_Stack.push("run_to_db()")
              xalt.run_to_db(rmapT, resultT)
              XALT_Stack.pop()
              runCnt += 1
            else:
              print("Error in xalt_syslog_to_db")
            XALT_Stack.pop()
          except:
            badCnt += 1
            # figure out length of array[2], as it might be 
            # near the syslog message size limit
            strg=array[2]
            lens = len(strg)
#            lenx = lens - (lens % 4 if lens % 4 else 4)
            print("xalt_syslog_to_db: undecodable entry!  length: ",lens)
#            resultT = base64.decodestring(strg[:lenx])
#            print("result:  ",result)

        count += 1
        pbar.update(count)

    pbar.fini()

#  what should be done if there are errors?
#    what went wrong?
#    how do we restart?
#
#  xalt.connect().close()

  t2 = time.time()
  rt = t2 - t1
  if (args.timer):
    print("Time: ", time.strftime("%T", time.gmtime(rt)))
#
  print("total processed : ", count, ", num links: ", lnkCnt, ", num runs: ", runCnt, ", badCnt: ", badCnt)

if ( __name__ == '__main__'): main()
