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
    parser.add_argument("--accept",   dest='accept',   action="store", default="accept.default.txt",   help="accept list file")
    parser.add_argument("--ignore",   dest='ignore',   action="store", default="ignore.default.txt",   help="ignore list file")
    parser.add_argument("--hostname", dest='hostname', action="store", default="hostname.default.txt", help="hostname acceptable list file")
    parser.add_argument("--input",    dest='input',    action="store", default="xalt_regex.template",  help="input template file")
    parser.add_argument("--output",   dest='output',   action="store", default="xalt_regex.h",         help="output header file")
    args = parser.parse_args()
    
    return args



def convert_file_to_string(path):

  try: 
    f = open(path,"r")
  except IOError as e:
    return ""

  a = []
  for line in f:
    idx  = line.find('#')
    if (idx == 0):
      continue
    if (idx != -1):
      line = line[0:idx-1]
    line = line.rstrip()
    if (len(line) == 0):
      continue
    a.append('"'+line+'"')
  f.close()

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
        line   = line[:idx-1] + toStr + line[idx+my_len:]
      
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

  ignoreStr = convert_file_to_string(args.ignore)
  acceptStr = convert_file_to_string(args.accept)
  hostStr   = convert_file_to_string(args.hostname)

  pattA = [
    ['@accept_list@',   acceptStr],
    ['@ignore_list@',   ignoreStr],
    ['@hostname_list@', hostStr],
  ]

  convert_template(pattA, args.input, args.output)


if ( __name__ == '__main__'): main()
