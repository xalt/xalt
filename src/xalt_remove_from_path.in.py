# -*- python -*-
#
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
from __future__ import print_function
import sys, os, re

patDoubleSlash   = re.compile(r'//+')
patTrailingSlash = re.compile(r'/$')
patDotSlash      = re.compile(r'/\.')

def main():
  xalt_bin   = sys.argv[1]
  xalt_bin   = patDotSlash.sub(     "/", xalt_bin)
  xalt_bin   = patDoubleSlash.sub(  "/", xalt_bin)
  xalt_bin   = patTrailingSlash.sub("",  xalt_bin)
  patXaltBin = re.compile("^" + re.escape(xalt_bin) + "$")

  pathA = os.environ["PATH"].split(":")
  for i , path in enumerate(pathA):
    path     = patDotSlash.sub(     "/", path)
    path     = patDoubleSlash.sub(  "/", path)
    path     = patTrailingSlash.sub("",  path)
    pathA[i] = path

  
  rA = []
  for path in pathA:
    m = patXaltBin.search(path)
    if (not m):
      rA.append(path)
  print(":".join(rA))


if ( __name__ == '__main__'): main()
