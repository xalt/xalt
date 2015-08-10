#!/usr/bin/env python
# -*- python -*-

from __future__ import print_function
import os, sys, re, time, datetime, argparse, ConfigParser, MySQLdb, base64
from Rmap_XALT import Rmap
from xalt_util import *

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--dryrun",       dest='dryrun',       action="store_true",  default = None,          help="dryrun")
    parser.add_argument("--dbname",       dest='dbname',       action="store",       default = "xalt",        help="db name")
    parser.add_argument("--syshost",      dest='syshost',      action="store",       default = "%",           help="system host name")
    args = parser.parse_args()
    return args


def dbConfigFn(dbname):
  """
  Build config file name from dbname.
  @param dbname: db name
  """
  return dbname + "_db.conf"


def fix_module_name(args, tableName, idName, cursor):

  cursor.execute("START TRANSACTION")
  query = "Select %s, module_name from %s where module_name like \"'%'\" " % (
    idName, tableName)

  if (args.syshost is not None):
    query = query + " AND syshost = '%s'" % args.syshost

  cursor.execute(query)
  resultA  = cursor.fetchall()
  n_update = 0
  for idx, module_name in resultA:
    n_update += 1
    mname = module_name[1:-1]
    if (args.dryrun):
      print ('path    : ' + path)
      print ('index   : ' + str(idx))
      print ('module  : ' + module_name)
      print ('SQL     : ' + query)
      print ('')
    else:
      query = "update "+tableName+" set module_name = %s where "+idName+" = %s" 
      cursor.execute(query, ( mname, idx))
  if (args.dryrun):
    print ('Found %d entries out of %d to update' % (n_update, len(resultA)))
  else:
    cursor.execute("COMMIT")
    print ('Updated %d entries out of %d objects' % (n_update, len(resultA)))
  

def main():
  args     = CmdLineOptions().execute()
  config   = ConfigParser.ConfigParser()     
  configFn = dbConfigFn(args.dbname)
  config.read(configFn)

  conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
  cursor = conn.cursor()

  XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR","./")
  ConfigFn     = os.path.join(XALT_ETC_DIR,"xalt_db.conf")
  RMapFn       = os.path.join(XALT_ETC_DIR,"reverseMapD")
  reverseMapT  = Rmap(RMapFn).reverseMapT()
  
  fix_module_name(args, "xalt_object", "obj_id", cursor)
  fix_module_name(args, "xalt_run",    "run_id", cursor)
  

if ( __name__ == '__main__'): main()
  



