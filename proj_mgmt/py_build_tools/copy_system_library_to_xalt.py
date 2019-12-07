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
from fnmatch     import fnmatch
from collections import OrderedDict 
import os, sys, re, argparse

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--lib64",    dest='lib64',    action="store", help="LIB64 install directory")
    parser.add_argument("--base",     dest='base',     action="store", help="base library")
    parser.add_argument("--real",     dest='real',     action="store", help="real library")
    args = parser.parse_args()
    
    return args

def files_in_tree(path,pattern):
  fileA = []
  if (not os.path.isdir(path)):
    return fileA

  for root, dirs, files in os.walk(path):
    for name in files:
      fn = os.path.join(root, name)
      if (fnmatch(fn,pattern)):
        fileA.append(fn)
  return fileA  

def main():

  args      = CmdLineOptions().execute()
  lib64_dir = args.lib64
  baseLib   = args.base
  realLib   = args.real

  baseBn    = os.path.basename(baseLib)
  dirNm     = os.path.realpath(os.path.dirname(baseLib))
  pattern   = os.path.join(dirNm,baseBn)+"*"
  fileA     = files_in_tree(dirNm, pattern)
  fileA.append(realLib)
  fileT     = {}
  for fn in fileA:
    fileT[fn] = True

  for fn in fileT:
    print("Storing "+fn+":")
    if (os.path.islink(fn)):
      newFn = os.readlink(fn)
      if (newFn.find('/') == -1):
        newFn = os.path.join(dirNm,newFn)
      if (os.path.isfile(newFn)):
        cmd = "cp "+newFn+" "+lib64_dir
        print (" ",cmd)
        os.system(cmd)
        cmd = "ln -sf "+os.path.basename(newFn)+" "+ os.path.join(lib64_dir,os.path.basename(fn))
        print (" ",cmd)
        os.system(cmd)

      else:
        print ("Cannot deal w/link: ",newFn)
        os.exit(-1)
    else:
      cmd = "cp "+fn+" "+lib64_dir
      print (" ",cmd)
      os.system(cmd)


if ( __name__ == '__main__'): main()
