
from __future__ import print_function
import os, sys, base64
import MySQLdb, argparse
import time
from operator import itemgetter
from datetime import datetime, timedelta
try:
  import configparser
except:
  import ConfigParser as configparser

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--confFn",  dest='confFn',    action="store",       default = "xalt_db.conf", help="db name")
    parser.add_argument("--start",   dest='startD',    action="store",       default = None,           help="start date")
    parser.add_argument("--end",     dest='endD',      action="store",       default = None,           help="end date")
    parser.add_argument("--syshost", dest='syshost',   action="store",       default = "%",            help="syshost")
    args = parser.parse_args()
    return args


def find_link_files(cursor, args, startdate, enddate):
  query = "SELECT exec_path from xalt_link \
                  where build_user = 'mclay' "
  cursor.execute(query)
  a = cursor.fetchall()
  resultA = []
  for file in a:
    if (os.path.isfile(file)):
      resultA.append(file)
      
  return resultA  


def main():
  XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR","./")
  args         = CmdLineOptions().execute()
  config       = configparser.ConfigParser()     
  configFn     = os.path.join(XALT_ETC_DIR,args.confFn)
  config.read(configFn)

  conn = MySQLdb.connect              \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
  cursor = conn.cursor()

  enddate = time.strftime('%Y-%m-%d')
  if (args.endD is not None):
    enddate = args.endD
  
  startdate = (datetime.strptime(enddate, "%Y-%m-%d") - timedelta(90)).strftime('%Y-%m-%d');
  if (args.startD is not None):
    startdate = args.startD

  resultA = find_link_files(cursor, args, startdate, enddate)

  for file in resultA:
    print(file)

if ( __name__ == '__main__'): main()
