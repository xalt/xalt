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
# XALT: A tool to track the programs on a cluster.
# Copyright (C) 2013-2017 Robert McLay and Mark Fahey
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

from __future__    import print_function
import os, sys, re, time

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from progressBar   import ProgressBar

class MY_FILEH(object):
  def __init__(self, fileName_pattern):
    self.__fileHandle = {}
    self.__pattern    = fileName_pattern

  def get_file_handle(self,syshost):
    if (not syshost in self.__fileHandle):
      fn = self.__pattern + "-" + syshost
      self.__fileHandle[syshost] = open(fn,"w")
    return self.__fileHandle[syshost]

  def close_file_handles(self):
    for fh in self.__fileHandle:
      self.__fileHandle[fh].close()


def main():
  fileName       = sys.argv[1]
  fileH          = MY_FILEH(fileName)
  syshostPattern = re.compile(r'syshost:([^ ]*) ')
  fnSz           = os.path.getsize(fileName)

  pbar           = ProgressBar(maxVal=fnSz)
  sz             = 0
  t1             = time.time()

  with open(fileName) as f:
    for line in f:
      sz += len(line)
      pbar.update(sz)
      m = syshostPattern.search(line)
      if (not m):
        continue
      syshost = m.group(1)
      fh = fileH.get_file_handle(syshost)
      fh.write(line)

  fileH.close_file_handles()
  pbar.fini()
  t2 = time.time()
  print("Time: ", time.strftime("%T", time.gmtime(t2-t1)))

if ( __name__ == '__main__'): main()
