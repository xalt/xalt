#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

#-----------------------------------------------------------------------
# XALT: A tool to track the programs on a cluster.
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

#  xalt_json_to_db takes the output found in the ~/.xalt.d/[link,run]*
#  output files and puts it into the database
#
#  optional input:
#    XALT_USERS:  colon separated list of users; only these users are 
#       considered instead of all
#

from __future__  import print_function
import os, sys, re, MySQLdb, json, time, argparse

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from XALTdb        import XALTdb
from xalt_util     import *
from xalt_global   import *
import warnings
warnings.filterwarnings("ignore", "Unknown table.*")

logger       = config_logger()

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--dbname",    dest='dbname',   action="store", default="xalt",       help="Name of the database")
    parser.add_argument("--results",   dest='resultFn', action="store", default="result.csv", help="result filename")
    parser.add_argument("--runs",      dest='runs',     action="store", default="2",          help="number of runs required to pass")
    parser.add_argument("--links",     dest='links',    action="store", default="1",          help="number of links required to pass")
    parser.add_argument("--functions", dest='nfuncs',   action="store", default="0",          help="number of functions required to pass")
    parser.add_argument("--pkgs",      dest='pkgs',     action="store", default="0",          help="number of packages required to pass")
    
    args = parser.parse_args()
    return args

def count_rows(conn, tableName):
  count = 0
  query  = "SELECT COUNT(*) FROM "+tableName
  conn.query(query)
  result = conn.store_result()
  if (result.num_rows() > 0):
    row   = result.fetch_row()
    count = int(row[0][0])
  return count


def main():
  """
  Check the entries for XALT db.
  """

  args   = CmdLineOptions().execute()
  xalt   = XALTdb(dbConfigFn(args.dbname))

  conn   = xalt.connect()
  query  = "USE "+xalt.db()
  conn.query(query)

  tableA = [ 'join_link_object', 'join_run_env', 'join_run_object', \
             'xalt_env_name', 'xalt_link', 'xalt_object', 'xalt_run',
             'xalt_function', 'join_link_function','xalt_pkg']
  tableT = {}
  for tableName in tableA:
    count = count_rows(conn, tableName)
    tableT[tableName] = count
    print(tableName,":", count)

  conn.close()

  result = 'diff'
  num    = int(args.runs)
  nfuncs = int(args.nfuncs)
  nlinks = int(args.links)
  pkgs   = int(args.pkgs)
  if (tableT['xalt_link']       == nlinks and
      tableT['xalt_run']        == num    and
      tableT['xalt_pkg']        == pkgs   and
      tableT['xalt_function']   >= nfuncs and
      tableT['xalt_object']     >   10    and
      tableT['xalt_env_name']   >   4 ):

    result = "passed"
  else:
    print("Expected:")
    print(" links:       ",nlinks)
    print(" runs:        ",num)
    print(" pkgs:        ",pkgs)
    print(" functions >= ",nfuncs)
    print(" objects   >  10")
    print(" env names >  4")

  f = open(args.resultFn,"w")
  f.write(result+"\n")
  f.close()
    
if ( __name__ == '__main__'): main()
