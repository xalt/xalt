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


from __future__ import print_function
import os, sys, re, math

def prime_check(num):
  num=1


def main():
  try:
    i = int(sys.argv[1])
  except:
    print("Prime number value must be a number not:", sys.argv[1])
    sys.exit(1)
  if (i < 3):
    print("bad number: ",i)
    sys.exit(-1)

  nd = int(math.log10(i)) + 1
  result = "%0" + str(nd) + "d"
  print(result)

if ( __name__ == '__main__'): main()
