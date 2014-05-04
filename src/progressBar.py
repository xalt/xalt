#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function, division
import os, sys, re

def getTerminalSize():
  """
  returns (lines:int, cols:int)
  """
  import os, struct
  def ioctl_GWINSZ(fd):
      import fcntl, termios
      return struct.unpack("hh", fcntl.ioctl(fd, termios.TIOCGWINSZ, "1234"))
  # try stdin, stdout, stderr
  if (not sys.stdout.isatty()):
    return (25, 80)

  fd = 1
  try:
    return ioctl_GWINSZ(fd)
  except:
    pass
  # try os.ctermid()
  try:
    fd = os.open(os.ctermid(), os.O_RDONLY)
    try:
      return ioctl_GWINSZ(fd)
    finally:
      os.close(fd)
  except:
    pass
  # try `stty size`
  try:
    return tuple(int(x) for x in os.popen("stty size", "r").read().split())
  except:
    pass
  # try environment variables
  try:
    return tuple(int(os.getenv(var)) for var in ("LINES", "COLUMNS"))
  except:
    pass
  # i give up. return default.
  return (25, 80)

class ProgressBar(object):
  def __init__(self, termWidth=None, barWidth=None, maxVal=None, ttyOnly=False, fd=sys.stderr):

    if (not maxVal):
      ValueError('Must specify maxVal')

    self.__maxVal = maxVal
    if (not barWidth):
      barWidth = termWidth
      if (not barWidth):
        r, barWidth = getTerminalSize()
        if (barWidth < 40):
          barWidth = 40

    barWidth = min(barWidth, self.__maxVal)

    self.__active   = True
    if (ttyOnly and not sys.stdout.isatty()):
      self.__active = False
      


    self.__fd       = fd
    self.__barWidth = barWidth

    self.__unit     = 100/self.__barWidth
    self.__fence    = self.__unit
    self.__mark     = 10
    self.__count    = -1
    self.__symbolT  = [ '+', '+', '+', '+', '|', '+', '+', '+', '+', '|' ]


  def update(self, i):
    if (not self.__active):
      return

    j = 100*i//self.__maxVal
    k = 100*(i+1)//self.__maxVal

    #print("i: ", i, "j: ",j,"k:",k, "fence: ", self.__fence,"mark:",self.__mark)

    if (j >= self.__fence):
      symbol = "-"
      if (( j <= self.__mark and k >= self.__mark) or
          ( j == k and j == self.__mark)         or
          (self.__fence > self.__mark)):
        
        self.__count +=  1
        self.__mark  += 10
        symbol = self.__symbolT[self.__count]
      self.__fd.write(symbol)
      self.__fence += self.__unit
  def fini(self):
    if (not self.__active):
      return
    self.__fd.write("\n")
        
def main():

    num = 200
    pbar = ProgressBar(maxVal=num, ttyOnly=False)
    for i in xrange(num):
      pbar.update(i+1)
    pbar.fini()  

if ( __name__ == '__main__'): main()
      
