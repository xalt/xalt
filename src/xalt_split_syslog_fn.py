#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, re

class FILEH(object):
  def __init__(self, fileName_pattern):
    self.__fileHandle = {}
    self.__pattern    = fileName_pattern

  def get_file_handle(self,syshost):
    fileHandle = self.__fileHandle
    if (not syshost in fileHandle):
      fn = self.__pattern + "-" + syshost
      fileHandle[syshost] = open(fn,"w")
    return fileHandle[syshost]

  def close_file_handles(self):
    for fh in self.__fileHandle:
      fh.close()


def main():
  fileName = sys.argv[1]

  fileH          = FILEH(fileName)
  sysHostPattern = re.compile(r'syshost:(.*) ')


  with open(fileName) as f:
    for line in f:
      m = syshostPattern.search(line)
      if (not m):
        continue
      syshost = m.group(1)
      fh = fileH(syshost)
      fh.write(line)

  fileH.close_file_handles()



if ( __name__ == '__main__'): main()
