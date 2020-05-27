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

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--confFn",      dest='confFn',     action="store", help="python config file")
    parser.add_argument("--input",       dest='input',      action="store", help="input template file")
    parser.add_argument("--output",      dest='output',     action="store", help="output header file")
    args = parser.parse_args()
    
    return args


def convert_pattern(list):
  a = []
  for entry in list:
    token = entry[0]
    value = entry[1].replace("\\","\\\\")
    
    a.append('"'+token+' => '+value+'"')

  return ",".join(a)

def convert_template(a, inputFn, outputFn):
  try:
    f = open(inputFn,"r")
  except IOError as e:
    print("Unable to open \"%s\", aborting!" % (inputFn))
    sys.exit(-1)

  
  outA = []
  for line in f:
    for entry in a:
      fromStr = entry[0]
      toStr   = entry[1]
      idx     = line.find(fromStr)
      if (idx != -1):
        my_len = len(fromStr)
        line   = line[:idx] + toStr + line[idx+my_len:]
      
    outA.append(line)

  f.close()
  
  try:
    of = open(outputFn,"w")
  except IOError as e:
    print("Unable to open \"%s\", aborting!" % (outputFn))
    sys.exit(-1)

  of.write("".join(outA))
  of.close()

def main():

  args = CmdLineOptions().execute()

  namespace = {}
  exec(open(args.confFn).read(), namespace)

  hostPatternStr = convert_pattern( namespace.get('hostname_patterns',       {}))
  pathPatternStr = convert_pattern( namespace.get('path_patterns',           {}))
  envPatternStr  = convert_pattern( namespace.get('env_patterns',            {}))

  pattA = [
    ['@hostname_patterns@',        hostPatternStr],
    ['@path_patterns@',            pathPatternStr],
    ['@env_patterns@',             envPatternStr]
  ]

  convert_template(pattA, args.input, args.output)


if ( __name__ == '__main__'): main()
