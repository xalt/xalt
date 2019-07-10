#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, re, math

def main():
  i = int(sys.argv[1])
  if (i < 3):
    print("bad number: ",i)
    sys.exit(-1)

  nd = int(math.log10(i)) + 1
  result = "%0" + str(nd) + "d"
  print(result)

if ( __name__ == '__main__'): main()
