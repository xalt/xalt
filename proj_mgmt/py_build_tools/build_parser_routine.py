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
    parser.add_argument("--default_dir", dest='defaultDir', action="store", help="xalt default dir", )
    parser.add_argument("--confFn",      dest='confFn',     action="store", help="python config file")
    parser.add_argument("--xalt_cfg",    dest='xaltCFG',    action="store", help="XALT std config")
    parser.add_argument("--pattern",     dest='pattern',    action="store", help="name of pattern")
    parser.add_argument("--input",       dest='input',      action="store", help="input template file")
    parser.add_argument("--output",      dest='output',     action="store", help="output file")
    args = parser.parse_args()
    
    return args

def convert_template(pattern, replaceA  ,inputFn, outputFn):
  try:
    f = open(inputFn,"r")
  except IOError as e:
    print("Unable to open \"%s\", aborting!" % (inputFn))
    sys.exit(-1)

  
  outA = []
  for line in f:
    idx = line.find(pattern)
    if (idx == -1):
      outA.append(line)
    else:
      for entry in replaceA:
        regexp = entry[1]
        value  = entry[0]
        line   = regexp + "  { return " + value + "; }\n"
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
  replaceA   = namespace.get(args.pattern, [])

  # If the --default_dir option is given then add XALT_DEFAULT_DIR to the list of paths to ignore.
  if (args.defaultDir):
    replaceA.append(['SKIP', '^'+args.defaultDir.replace('/',r'\/')+r'\/.*'])


  namespace = {}
  exec(open(args.xaltCFG).read(), namespace)
  replaceA.extend(namespace.get(args.pattern, []))

  convert_template("@" + args.pattern + "@", replaceA, args.input, args.output)

if ( __name__ == '__main__'): main()
