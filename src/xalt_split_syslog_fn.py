#!/usr/bin/env python
# -*- python -*-
from __future__    import print_function
import os, sys, re, time
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
