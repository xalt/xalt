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

from __future__                import print_function
import os, sys

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from xalt_util                 import *
from xalt_transmission_factory import XALT_transmission_factory
from xalt_global               import *

logger    = config_logger()
parenPat  = re.compile(r'.*\((.*)\).*')
tmpObjPat = re.compile(r'/tmp/[_a-zA-Z0-9-]+.o')

def cleanup(xaltobj, fn):
  """
  cleanup the output of ld --trace 
  @param xaltobj: The name of the XALT object file.
  @param fn:      The file path that contains the ld --trace output.
  """
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
      s    = os.path.realpath(m.group(1))
      d[s] = True
      continue
  
    # Save everything else
    idx = s.find("\n")
    if (idx != -1):
      s = s[:idx]

    s = os.path.realpath(s)
    d[s] = True

  # make the list unique  
  sA = d.keys()
  
  sA = sorted(sA)

  sB = []
  for lib in sA:
    hash_line = capture(['sha1sum', lib])
    if (hash_line.find("No such file or directory") != -1):
      continue
    else:
      v = hash_line.split()[0]
    sB.append([lib, v])

  return sB
    
def main():
  """
  Built a json output of the ld --trace data.
  """
  User = os.environ.get("USER",  "unknown")
  Host = os.environ.get("HOSTNAME","unknown")
  # push User, host and command line on to XALT_Stack
  XALT_Stack.push("User: " + User)
  XALT_Stack.push("Host: " + Host)
  sA = []
  sA.append("CommandLine:")
  for v in sys.argv:
    sA.append('"'+v+'"')

  s = " ".join(sA)
  XALT_Stack.push(s)

  try:
    uuid        = sys.argv[ 1]
    status      = sys.argv[ 2]
    wd          = sys.argv[ 3]
    syshost     = sys.argv[ 4]
    execname    = sys.argv[ 5]
    xaltobj     = sys.argv[ 6]
    build_epoch = sys.argv[ 7]
    linklineFn  = sys.argv[ 8]
    resultFn    = sys.argv[ 9]

    if (execname.find("conftest") != -1):
      return 1
  
    hash_line   = capture(['sha1sum', execname])  
    if (hash_line.find("No such file or directory") != -1):
      return 1
    hash_id     = hash_line.split()[0]

    # Step one clean up linkline data
    sA = cleanup(xaltobj, linklineFn)
  
    resultT                  = {}
    resultT['uuid']          = uuid
    resultT['link_program']  = extract_compiler()
    resultT['build_user']    = User
    resultT['exit_code']     = int(status)
    resultT['build_epoch']   = float(build_epoch)
    resultT['exec_path']     = os.path.abspath(execname)
    resultT['hash_id']       = hash_id
    resultT['wd']            = wd
    resultT['build_syshost'] = syshost
    resultT['linkA']         = sA
    
    xfer  = XALT_transmission_factory.build(XALT_TRANSMISSION_STYLE,
                                            syshost, "link", resultFn)
    xfer.save(resultT)

  except Exception as e:
    print("XALT_EXCEPTION(xalt_generate_linkdata.py): ",e)
    logger.exception("XALT_EXCEPTION:xalt_generate_linkdata"+XALT_Stack.contents())

  return 0

if ( __name__ == '__main__'):
  iret = main()
  sys.exit(iret)
