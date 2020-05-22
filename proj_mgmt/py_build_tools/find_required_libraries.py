#!/bin/sh
# -*- python -*-

################################################################################
# This file is python bilingual: The next line starts a comment in Python,
# and is a no-op in shell
""":"
# Find a suitable python interpreter (adapt for your specific needs) 
for cmd in python3 python python2; do
   command -v > /dev/null $cmd && exec $cmd $0 "$@"
done

echo "Error: Could not find a valid python interpreter --> exiting!" >&2
exit 2
":"""
################################################################################

# Git Version: @git@

#-----------------------------------------------------------------------
# XALT: A tool that tracks users jobs and environments on a cluster.
# Copyright (C) 2013-2014 University of Texas at Austin
# Copyright (C) 2013-2014 University of Tennessee
# 
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of 
# the License, or (at your option) any later version. 
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser  General Public License for more details. 
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA
#-----------------------------------------------------------------------

from __future__  import print_function
import os, sys, re, subprocess


ignoreT = {
  "libc"      : True,
  "libuuid"   : True,
  "libcurl"   : True,
  "libcrypto" : True,
  }
  


def find_libs(libT, execNm):
  cmd    = "ldd " + execNm
  p      = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                            stderr = subprocess.STDOUT, shell =True)
  output = p.communicate()[0]
  lineA  = output.decode("utf-8").splitlines()
  patt   = re.compile(r"(.*).so")
  for line in lineA:
    line   = re.sub("\(.*\)","",line)
    fieldA = line.split()
    N      = len(fieldA)
    if (N < 3):
      continue
    if (fieldA[1] == "=>"):
      my_name = fieldA[0]
      m       = patt.match(my_name)
      if (m):
        my_name = m.group(1)

      if (not( my_name in ignoreT)):
        lib     = fieldA[2]
        libT[lib] = True
      


def main():
  numArgs = len(sys.argv)
  libT    = {}

  for i in range(1,numArgs):
    lib   = sys.argv[i]
    if (not os.path.exists(lib)):
      print(lib+": does not exist!")
      continue
    find_libs(libT, lib)

  for k in libT:
    print(k)

if ( __name__ == '__main__'): main()
