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
    parser.add_argument("--xalt_cfg",    dest='xaltCFG',    action="store", help="XALT std config")
    parser.add_argument("--default_dir", dest='defaultDir', action="store", help="xalt default dir", )
    parser.add_argument("--input",       dest='input',      action="store", help="input template file")
    parser.add_argument("--output",      dest='output',     action="store", help="output header file")
    args = parser.parse_args()
    
    return args


def convert_pattern(list):
  a = []
  for entry in list:
    token = str(entry[0])
    value = entry[1].replace("\\","\\\\")
    
    a.append('"'+token+' => '+value+'"')

  return a

def convert_py_pkg_pattern(list):
  a = []
  for entry in list:
    k_s  = entry['k_s'].replace("\\","\\\\")
    kind = entry['kind'].replace("\\","\\\\")
    patt = entry['patt'].replace("\\","\\\\")
    a.append('"'+"{ 'k_s' : '"+k_s+"', 'kind' : '"+kind+"', 'patt' : r'"+patt+"'},"+'"')

  return a

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

  # Read and process site configuration file.

  namespace = {}
  exec(open(args.confFn).read(),      namespace)
  hostStrA   = convert_pattern(        namespace.get('hostname_patterns',   []))
  pathStrA   = convert_pattern(        namespace.get('path_patterns',       []))
  envStrA    = convert_pattern(        namespace.get('env_patterns',        []))
  pyPkgStrA  = convert_py_pkg_pattern( namespace.get('python_pkg_patterns', []))
  ingestStrA = convert_pattern(        namespace.get('pre_ingest_patterns', []))
  
  # Read and process the XALT configuration file that provides the defaults.

  hostStrA.append(   '"===="' )
  pathStrA.append(   '"===="' )
  envStrA.append(    '"===="' )
  pyPkgStrA.append(  '"===="' )
  ingestStrA.append( '"===="' )

  # If the --default_dir option is given then add XALT_DEFAULT_DIR to the list of paths to ignore.
  if (args.defaultDir):
    pattDefDir = '^'+args.defaultDir.replace('/',r'\/')+r'\/.*'
    pathStrA.extend(convert_pattern([ ['SKIP', pattDefDir] ]))

  namespace = {}
  exec(open(args.xaltCFG).read(),            namespace)
  hostStrA.extend(   convert_pattern(        namespace.get('hostname_patterns',   [])))
  pathStrA.extend(   convert_pattern(        namespace.get('path_patterns',       [])))
  envStrA.extend(    convert_pattern(        namespace.get('env_patterns',        [])))
  pyPkgStrA.extend(  convert_py_pkg_pattern( namespace.get('python_pkg_patterns', [])))
  ingestStrA.extend( convert_pattern(        namespace.get('pre_ingest_patterns', []))

  pattA = [
    ['@hostname_patterns@',        ",".join(hostStrA)],
    ['@path_patterns@',            ",".join(pathStrA)],
    ['@env_patterns@',             ",".join(envStrA)],
    ['@python_pkg_patterns@',      ",".join(pyPkgStrA)],
    ['@pre_ingest_patterns@',      ",".join(ingestStrA)],
  ]

  convert_template(pattA, args.input, args.output)


if ( __name__ == '__main__'): main()
