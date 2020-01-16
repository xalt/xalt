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

from __future__ import print_function
import os, sys, re, json

def main():

  fn = sys.argv[1]
  t  = {}

  with open(fn) as fh:
    for line in fh:
      idx = line.find(",")
      if (idx == -1):
        print("error in line: ",line)
        os.exit(-1)

      chgStr = line[:idx]
      FoS    = line[idx+1:].strip().replace(",","")
      if (FoS == "Fluid Particulate and Hyrdaulic Systems"):
        FoS = "Fluid Particulate and Hydraulic Systems"
      if (FoS != "Unknown"):
        t[chgStr] = FoS

  output = json.dumps(t, sort_keys=True, indent=4, separators=(',', ': ') )

  print (output)
  


if ( __name__ == '__main__'): main()

