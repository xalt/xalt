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
import os, sys, re

def main():
  for i in sys.argv:
    fn = os.path.abspath(i)
    if (os.path.exists(fn)):
      print (i, fn)
    else:
      print (i)
      


if ( __name__ == '__main__'): main()
