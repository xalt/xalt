#!/usr/bin/env python
# -*- python -*-

from __future__ import print_function
import os, sys, re, time, datetime, argparse, ConfigParser, MySQLdb
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
    parser.add_argument("--dbname",       dest='dbname',       action="store", default = "xalt",        help="db name")
    parser.add_argument("--syshost",      dest='syshost',      action="store", default = "%",           help="system host name")
    parser.add_argument("--rmapD",        dest='rmapD',        action="store", default = "reverseMapD", help="Reverse Map Directory")
    parser.add_argument("--exclude_path", dest='exclude_path', action="append",                         help="exclude objects whose paths partially match EXCLUDE_PATH. This option can be specified multiple times.")
    args = parser.parse_args()
    return args


def dbConfigFn(dbname):
  """
  Build config file name from dbname.
  @param dbname: db name
  """
  return dbname + "_db.conf"


def update_module_name(args, tableName, idName, pathName, reverseMapT, cursor):

  cursor.execute("START TRANSACTION")
  query = "Select %s, %s from %s where module_name is NULL" % (
    idName, pathName, tableName)

  if (args.syshost is not None):
    query = query + " AND syshost = '%s'" % args.syshost

  if (args.exclude_path is not None):
    for path in args.exclude_path:
      query = query + " AND %s not like '%%%s%%'" % (pathName, path)
  
  cursor.execute(query)
  resultA  = cursor.fetchall()
  n_update = 0
  for idx, path in resultA:
    module_name = obj2module(path, reverseMapT)
    if (module_name != 'NULL'):
      n_update += 1
      query = "update "+tableName+" set module_name = '%s' where "+idName+" = '%s'" % ( 
        module_name, idx)
      if (args.dryrun):
        print ('obj_path: ' + obj_path)
        print ('module  : ' + module_name)
        print ('SQL     : ' + query)
        print ('')
      else:
        cursor.execute(query)
  if (args.dryrun):
    print ('Found %d entries out of %d to update' % (n_update, len(results)))
  else:
    conn.commit()
    print ('Updated %d entries out of %d objects' % (n_update, len(results)))
  

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
  
  update_module_name(args, "xalt_object", "obj_id", "object_path", reverseMapT, cursor)
  update_module_name(args, "xalt_run",    "run_id", "exec_path",   reverseMapT, cursor)
  

if ( __name__ == '__main__'): main()
  



