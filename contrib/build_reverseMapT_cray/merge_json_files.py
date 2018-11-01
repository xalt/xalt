#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, re, json

def main():

  masterT = {}

  for argv in sys.argv[1:]:
    f = open(argv,"r")
    t = json.loads(f.read())
    f.close()

    rmapT = t['reverseMapT']

    for k in rmapT:
      masterT[k] = rmapT[k]
 
  t = { "reverseMapT" : masterT,
        "timestampFn" : False,
      }

  s = json.dumps(t, sort_keys=True, indent=2, separators=(',',': '))
  print(s)




if ( __name__ == '__main__'): main()
