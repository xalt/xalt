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
import os, sys, re, json, tempfile

def main():
  syshost=sys.argv[1]
  jsonStr=sys.argv[2]

  print("syshost: ",syshost)
  
  cwd    = os.getcwd() # Get the current working directory
  tbl    = json.loads(jsonStr.encode("ascii","ignore"))
  prefix = os.path.join(cwd, ".xalt.d", tbl.get("rec_type") + "." + syshost + ".")


  # Pattern: diagnostic_<random>_report.csv
  fd, path = tempfile.mkstemp(prefix=prefix, suffix=".json")

  with os.fdopen(fd, 'w') as tmp_file:
    tmp_file.write(jsonStr)


if ( __name__ == '__main__'): main()
