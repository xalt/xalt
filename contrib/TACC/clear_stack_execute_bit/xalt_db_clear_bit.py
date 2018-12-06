
from __future__ import print_function
import os, sys, base64
import MySQLdb, argparse
import time, subprocess
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
    parser.add_argument("--confFn",    dest='confFn',    action="store",       default = "xalt_db.conf", help="db name")
    parser.add_argument("--start",     dest='startD',    action="store",       default = None,           help="start date")
    parser.add_argument("--end",       dest='endD',      action="store",       default = None,           help="end date")
    parser.add_argument("--syshost",   dest='syshost',   action="store",       default = "%",            help="syshost")
    parser.add_argument("--clear_bit", dest='clear_bit', action="store_true",  default = False,          help="Clear bit")
    args = parser.parse_args()
    return args


def find_link_files(cursor, args, startdate, enddate):
  query = "SELECT exec_path from xalt_link \
                  where build_user = 'mclay' "
  cursor.execute(query)
  a = cursor.fetchall()
  resultT = {}
  for entry in a:
    file = entry[0]
    if (os.path.isfile(file)):
      resultT[file] = True
      
  return resultT

def capture(cmd):
  """
  Capture standard out for a command.
  @param cmd: Command string or array.
  """

  if (type(cmd) == type(' ')):
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT, shell =True)
  else:
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT)

    
  return p.communicate()[0]

def clear_stack_bit(file):
  result = capture("execstack " + file)
  if (result[0:1] == 'X'):
    os.system("execstack -c " + file)
    print("clearing stack bit on: ",file)

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

  resultT = find_link_files(cursor, args, startdate, enddate)

  for file in resultT:
    if (args.clear_bit):
      clear_stack_bit(file)
    else:
      print(file)

if ( __name__ == '__main__'): main()
