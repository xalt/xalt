#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import platform

def syshost():
  hostA = platform.node().split('.')
  idx = 1 
  if (len(hostA) < 2):
    idx = 0
  return hostA[idx]

def main():
  print(syshost())
  

if ( __name__ == '__main__'): main()
