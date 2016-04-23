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
import os, sys, re, MySQLdb, json, time, argparse, base64, zlib, shlex

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from XALTdb        import XALTdb
from xalt_site_pkg import translate
from xalt_util     import *
from xalt_global   import *
from progressBar   import ProgressBar
from Rmap_XALT     import Rmap

import inspect

def __LINE__():
    try:
        raise Exception
    except:
        return sys.exc_info()[2].tb_frame.f_back.f_lineno

def __FILE__():
    return inspect.currentframe().f_code.co_filename

#print ("file: '%s', line: %d" % (__FILE__(), __LINE__()), file=sys.stderr)

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
    parser.add_argument("--syslog_file", dest='syslog',   action="store",      help="Location and name of syslog file")
    parser.add_argument("--leftover_fn", dest='leftover', action="store",      default='leftover.log',
                                                                               help="Name of the leftover file")
    parser.add_argument("--timer",       dest='timer',    action="store_true", help="Time runtime")
    parser.add_argument("--reverseMapD", dest='rmapD',    action="store",      help="Path to the directory containing the json reverseMap")
    parser.add_argument("--dbname",      dest='dbname',   action="store",      default="xalt", help="Name of the database")
    args = parser.parse_args()
    return args


class Record(object):
  """
  This class holds the pieces of a single record as it comes in from
  syslog
  """
  def __init__(self, t):
    nblks          = int(t['nb'])
    self.__nblks   = nblks
    self.__kind    = t['kind']
    self.__syshost = t['syshost']
    self.__blkCnt  = 0

    blkA = []
    for i in range(nblks):
      blkA.append(False)

    self.__blkA    = blkA
    self.addBlk(t)

  def addBlk(self,t):
    idx               = int(t['idx'])
    self.__blkA[idx]  = t['value']
    self.__blkCnt    += 1
    
  def completed(self):
    return (self.__blkCnt >= self.__nblks)

  def value(self):
    return "".join(self.__blkA)

  def prt(self, prefix, key):
    sA    = []
    nblks = self.__nblks
    blkA  = self.__blkA

    sPA   = []
    sPA.append(prefix)
    sPA.append(" kind:")
    sPA.append(self.__kind)
    sPA.append(" syshost:")
    sPA.append(self.__syshost)
    sPA.append(" key:")
    sPA.append(key)
    sPA.append(" nb:")
    sPA.append(str(nblks))
    ss = "".join(sPA)

    for idx in range(nblks):
      value = blkA[idx]
      if (value):
        sA.append(ss)
        sA.append(" idx:")
        sA.append(str(idx))
        sA.append(" value:")
        sA.append(value)
        sA.append("\n")
    return "".join(sA)

def parseSyslogV1(s):
  t = { 'kind' : None, 'value' : None, 'syshost' : None, 'version' : 1}

  idx = s.find("link:")
  if (idx == -1):
    idx = s.find("run:")
  if (idx == -1):
    return t, False

  array        = s[idx:].split(":")
  t['kind']    = array[0].strip()
  t['syshost'] = array[1].strip()
  t['value']   = base64.b64decode(array[2])
  return t, True

def parseSyslogV2(s, recordT):
  t = { 'kind' : None, 'syshost' : None, 'value' : None, 'version' : 2}

  idx = s.find(" V:2 ")
  if (idx  == -1):
    return t, False

  # Strip off "XALT_LOGGING V:2" from string.
  s                      = s[idx+5:]

  # Setup parser
  lexer                  = shlex.shlex(s)
  lexer.whitespace_split = True
  lexer.whitespace       = ' :'

  # Pick off two values at a time.
  try: 
    while True:
      key    = next(lexer)
      value  = next(lexer)
      t[key] = value
  except StopIteration as e:
    pass
  

  # get the key from the input, then place an entry in the *recordT* table.
  # or just add the block to the current record.
  key = t['key']
  r    = recordT.get(key, None)
  if (r):
    r.addBlk(t)
  else:
    r  = Record(t)
    recordT[key] = r

  # If the block is completed then grap the value, remove the entry from *recordT*
  # and return a completed table.
  if (r.completed()):
    
    rv   = r.value()
    b64v = base64.b64decode(rv)
    vv   = zlib.decompress(b64v)

    t['value'] = vv
    recordT.pop(key)
    return t, True

  # Entry is not complete.
  return t, False

def parseSyslog(s, recordT):
  if ("XALT_LOGGING" in s) and ("V:2" in s):
    return parseSyslogV2(s, recordT)
  return parseSyslogV1(s)


def main():
  """
  read from syslog file into XALT db.
  """

  sA = []
  sA.append("CommandLine:")
  for v in sys.argv:
    sA.append('"'+v+'"')
  XALT_Stack.push(" ".join(sA))

  args       = CmdLineOptions().execute()
  xalt       = XALTdb(dbConfigFn(args.dbname))
  syslogFile = args.syslog

  # should add a check if file exists
  num    = int(capture("cat "+syslogFile+" | wc -l"))
  icnt   = 0

  t1     = time.time()

  rmapT  = Rmap(args.rmapD).reverseMapT()

  lnkCnt = 0
  runCnt = 0
  badCnt = 0
  count  = 0

  recordT = {}

  if (num == 0):
    return
    
  pbar   = ProgressBar(maxVal=num)

  fnA = [ args.leftover, syslogFile ]

  for fn in fnA:
    if (not os.path.isfile(fn)):
      continue

    f=open(fn, 'r')
    for line in f:
      if (not ("XALT_LOGGING" in line)):
        continue
      t, done = parseSyslog(line, recordT)
      if (not done):
        continue

      try:
        XALT_Stack.push("XALT_LOGGING: " + t['kind'] + " " + t['syshost'])

        if ( t['kind'] == "link" ):
          XALT_Stack.push("link_to_db()")
          xalt.link_to_db(rmapT, json.loads(t['value']))
          XALT_Stack.pop()
          lnkCnt += 1
        elif ( t['kind'] == "run" ):
          XALT_Stack.push("run_to_db()")
          xalt.run_to_db(rmapT, json.loads(t['value']))
          XALT_Stack.pop()
          runCnt += 1
        else:
          print("Error in xalt_syslog_to_db", file=sys.stderr)
        XALT_Stack.pop()
      except Exception as e:
        print(e, file=sys.stderr)
        badCnt += 1

      count += 1
      pbar.update(count)

    f.close()

  pbar.fini()

  t2 = time.time()
  rt = t2 - t1
  if (args.timer):
    print("Time: ", time.strftime("%T", time.gmtime(rt)))
  print("total processed : ", count, ", num links: ", lnkCnt, ", num runs: ", runCnt, ", badCnt: ", badCnt)
        
  leftover = args.leftover
  if (os.path.isfile(leftover)):
    os.rename(leftover, leftover + ".old")
  
  # if there is anything left in recordT file write it out to the leftover file.

  if (recordT):
    f = open(leftover, "w")
    for key in recordT:
      r = recordT[key]
      s = r.prt("XALT_LOGGING V=2", key)
      f.write(s)
    f.close()


if ( __name__ == '__main__'): main()
