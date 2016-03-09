
# -*- python -*-
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
import os, sys, re, argparse

def hardcode(name,output):
  sA = []
  sA.append("const char * xalt_syshost() {")
  sA.append("  return \"" + name + "\";")
  sA.append("}")

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()


def read_file(name,output):
  sA = []
  sA.append("const char * xalt_syshost() {")
  sA.append("  return \"" + name + "\";")
  sA.append("}")

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()

  
def second_name(name,output):
  print("second_name")


def mapping(name,output):
  print("mapping")
  

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--input",    dest='input',    action="store", default="unknown",        help="input string")
    parser.add_argument("--output",   dest='output',   action="store", default="xalt_syshost.c", help="output c routine")
    args = parser.parse_args()
    
    return args

kindA = [ ['hardcode',True, hardcode ], ['second_name',False, second_name],
          ['read_file',True, read_file], ['mapping',True,mapping] ]

def main():
  args = CmdLineOptions().execute()

  # split argument at colon if there
  arg = args.input
  idx = arg.find(":")
  if (idx == -1):
    kind = arg.lower()
    opt  = None
  else:
    kind = arg[:idx].lower()
    opt  = arg[idx+1:]

  # Check for valid argument and possible option, set [[func]] to be the function to build

  found = False
  for entry in kindA:
    if (kind == entry[0]):
      if (entry[1] and not opt):
        print("Syshost config option \"%s\" does not have the required options" % kind)
        sys.exit(1)
      found = True
      func = entry[2]
      break

  if (not found):
    choices = ""
    for entry in kindA:
      choices += "\"" + entry[0] + "\", "
    print("Syshost config \"%s\" is not valid, it must be one of %s" % (kind, choices))
    sys.exit(1)


  # 

  func(opt, args.output)


  

  

    
  
  




if ( __name__ == '__main__'): main()
