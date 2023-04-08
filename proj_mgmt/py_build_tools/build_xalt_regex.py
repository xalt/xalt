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
import os, sys, re, argparse, traceback

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
    token = entry[0]
    value = entry[1].replace("\\","\\\\")
    
    a.append('"'+token+' => '+value+'"')

  return a

def convert_ingest_pattern(list):
  a = []
  for entry in list:
    token = "%.4f" % (entry[0])
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
  try:
    exec(open(args.confFn).read(), namespace)
  except:
    print("\nUnable to parse python config file: ",args.confFn,"-> see below:\n")
    print(traceback.format_exc())
    print("\nExiting!\n")
    sys.exit(1)

  hostStrA    = [ '"----"']
  pathStrA    = [ '"----"']
  pathArgStrA = [ '"----"']
  envStrA     = [ '"----"']
  pkgStrA     = [ '"----"']
  pyPkgStrA   = [ '"----"']
  ingestStrA  = [ '"----"']

  hostStrA.extend(    convert_pattern(        namespace.get('hostname_patterns',   [])))
  pathStrA.extend(    convert_pattern(        namespace.get('path_patterns',       [])))
  pathArgStrA.extend( convert_pattern(        namespace.get('path_arg_patterns',   [])))
  envStrA.extend(     convert_pattern(        namespace.get('env_patterns',        [])))
  pkgStrA.extend(     convert_pattern(        namespace.get('pkg_patterns',        [])))
  pyPkgStrA.extend(   convert_py_pkg_pattern( namespace.get('python_pkg_patterns', [])))
  ingestStrA.extend(  convert_ingest_pattern( namespace.get('pre_ingest_patterns', [])))
  
  # Read and process the XALT configuration file that provides the defaults.
  hostStrA.append(    '"===="' )
  pathStrA.append(    '"===="' )
  pathArgStrA.append( '"===="' )
  envStrA.append(     '"===="' )
  pkgStrA.append(     '"===="' )
  pyPkgStrA.append(   '"===="' )
  ingestStrA.append(  '"===="' )

  hd_pathStrA = ['"===="']

  namespace = {}
  try:
    exec(open(args.xaltCFG).read(),            namespace)
  except:
    print("\nUnable to parse python config file: ",args.xaltCFG,"-> see below:\n")
    print(traceback.format_exc())
    print("\nExiting!\n")
    sys.exit(1)
  hd_pathA = namespace.get('head_path_patterns',  [])
  if (args.defaultDir):
    found = False
    pattDefDir = '^'+args.defaultDir.replace('/',r'\/')+r'\/.*'
    for pair in hd_pathA:
      if (pair[1] == "XALT_INSTALL_DIR"):
        pair[1] = pattDefDir
        found= True
    if (not found):
      hd_pathStrA.extend(convert_pattern([ ['SKIP', pattDefDir] ]))
  hd_pathStrA.extend(convert_pattern(        hd_pathA))

  hostStrA.extend(    convert_pattern(        namespace.get('hostname_patterns',   [])))
  pathStrA.extend(    convert_pattern(        namespace.get('path_patterns',       [])))
  pathArgStrA.extend( convert_pattern(        namespace.get('path_arg_patterns',   [])))
  envStrA.extend(     convert_pattern(        namespace.get('env_patterns',        [])))
  pkgStrA.extend(     convert_pattern(        namespace.get('pkg_patterns',        [])))
  pyPkgStrA.extend(   convert_py_pkg_pattern( namespace.get('python_pkg_patterns', [])))
  ingestStrA.extend(  convert_ingest_pattern( namespace.get('pre_ingest_patterns', [])))
  # If the --default_dir option is given then add XALT_DEFAULT_DIR to the list of paths to ignore.
      
  pattA = [
    ['@hostname_patterns@',        ",".join(hostStrA)],
    ['@path_patterns@',            ",".join(pathStrA)],
    ['@path_arg_patterns@',        ",".join(pathArgStrA)],
    ['@head_path_patterns@',       ",".join(hd_pathStrA)],
    ['@env_patterns@',             ",".join(envStrA)],
    ['@pkg_patterns@',             ",".join(pkgStrA)],
    ['@python_pkg_patterns@',      ",".join(pyPkgStrA)],
    ['@pre_ingest_patterns@',      ",".join(ingestStrA)],
  ]

  convert_template(pattA, args.input, args.output)


if ( __name__ == '__main__'): main()
