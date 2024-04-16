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

#-----------------------------------------------------------------------
# XALT: A tool that tracks users jobs and environments on a cluster.
# Copyright (C) 2013-2015 University of Texas at Austin
# Copyright (C) 2013-2015 University of Tennessee
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

#----------------------------------------------------------------------
# xalt_rmap_lmap.py builds the reference libmap in the ReverseMapD file
# to be used with the function tracking feature. The RevereseMapD file
# needs to already exist in XALT_ETC_DIR.
#----------------------------------------------------------------------


import os, sys, json

from xalt_global import *

py_script = sys.argv[0]
tail      = py_script[-6:]
if (tail == ".in.py"):
  print("Please do not execute this version")
  print("Use the installed version instead")
  sys.exit(-1)

RMapD   = os.path.join(XALT_ETC_DIR,"reverseMapD")

rmapA   = RMapD.split(':')
searchA = [ ".json", ".old.json"]
rmapFn  = None   
for dir in rmapA:
  for ext in searchA:
    rmapFn = os.path.join(dir, "jsonReverseMapT" + ext)
    if (os.access(rmapFn, os.R_OK)):
      break 

if (rmapFn):
  fp    = open(rmapFn,"r")
  RMap  = json.loads(fp.read())
  RMapT = RMap['reverseMapT']
  fp.close()

  libmap = set()
  for key in RMapT:
    if RMapT[key]["kind"] == "lib":
      files = [ f for f in os.listdir(key) \
                if os.path.isfile(os.path.join(key,f)) ]
      for file in files:
        if file.endswith(('.a', '.so')):
          libmap.add(file)
  
  RMap["xlibmap"] = list(libmap)
  
  fp = open(rmapFn, "w")
  jd = json.dumps(RMap, sort_keys=True, indent=2, separators=(',', ': '))
  print >> fp, jd
  fp.close()
else:
  print "Cannot find ReverseMap file to use. Please build it first."
