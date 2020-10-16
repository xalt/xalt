#!/bin/sh
# -*- python -*-

################################################################################
# This file is python 2/3 bilingual. 
# The line """:" starts a comment in python and is a no-op in shell.
""":"
# Shell code to find and run a suitable python interpreter.
for cmd in python3 python python2; do
   command -v > /dev/null $cmd && exec $cmd $0 "$@"
done

echo "Error: Could not find a valid python interpreter --> exiting!" >&2
exit 2
":""" # this line ends the python comment and is a no-op in shell.
################################################################################

#------------------------------------------------------------------------    #
# This python script gets executable usage on SYSHOST grouped by exec name.  #
# Total CPU time used, number of jobs, and number of users of the exec are   #
# shown. Executable with "known" names are shown as all CAPS and grouped     #
# together even if they have different actual exec name / version, other     #
# executables are only grouped by their name.                                #
#                                                                            #
# Examples:                                                                  #
#                                                                            #
# 1. Show the help output:                                                   #
#   python executable_usage.py -h                                            #
#                                                                            #
# 2. Get executable usage on Darter for the last 90 days                     #
#   python executable_usage.py darter                                        #
#                                                                            #
# 3. Get executable usage on Darter for specific period                      #
#   python executable_usage.py darter --startdate 2015-03-01 \               #
#          --endate 2015-06-31                                               #
#                                                                            #
# 4. Get executable usage on Darter for the last 90 days, excluding all      #
#    module name with 'ari', 'gcc', and 'craype' in its name                 #
#   python executable_usage.py darter --exclude ari,gcc,craype               #
#                                                                            #
#----------------------------------------------------------------------------#

from __future__ import print_function
import os, sys, base64
import MySQLdb, argparse
import time, collections
from operator import itemgetter
from datetime import datetime, timedelta
try:
  import configparser
except:
  import ConfigParser as configparser

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from BeautifulTbl      import BeautifulTbl
from xalt_name_mapping import name_mapping

def shortName(full):
  idx = full.find('(')
  if (idx != -1):
    full = full[:idx]

  idx = full.rfind('/')
  if (idx == -1):
    return full
  return full[:idx]

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--confFn",      dest='confFn',     action="store",       default = "xalt_db.conf", help="db name")
    parser.add_argument("--sqlPattern",  dest='sqlPattern', action="store",       default = "%",            help="SQL Pattern")
    parser.add_argument("--start",       dest='startD',     action="store",       default = None,           help="start date")
    parser.add_argument("--end",         dest='endD',       action="store",       default = None,           help="end date")
    parser.add_argument("--syshost",     dest='syshost',    action="store",       default = "%",            help="syshost")
    parser.add_argument("--queue",       dest='queue',      action="store",       default = "%",            help="queue")
    parser.add_argument("--num",         dest='num',        action="store",       default = 20,             help="top number of entries to report")
    parser.add_argument("--full",        dest='full',       action="store_true",                            help="report core hours by compiler")
    parser.add_argument("--short",       dest='short',      action="store_true",                            help="Only report on executable, not libraries")
    args = parser.parse_args()
    return args

class Libraries:

  def __init__(self, cursor):
    self.__libA  = []
    self.__cursor = cursor

  def build(self, args, start_date, end_date, queue="%"):
    
    if (queue == "%"):
      queue = args.queue

    

    query = "select ROUND(SUM(t3.num_cores*t3.run_time/3600.0)) as corehours,        \
                    COUNT(DISTINCT(t3.user)) as n_users,                             \
                    COUNT(DISTINCT(t3.account)) as n_accts,                          \
                    COUNT(t3.date) as n_runs, COUNT(DISTINCT(t3.job_id)) as n_jobs,  \
                    t1.object_path, t1.module_name as module                         \
                    from xalt_object as t1, join_run_object as t2, xalt_run as t3    \
                    where t1.module_name like %s                                     \
                    and t1.obj_id = t2.obj_id and t2.run_id = t3.run_id              \
                    and t3.syshost like %s and t3.queue like %s                      \
                    and t3.date >= %s and t3.date < %s                               \
                    group by t1.object_path order by corehours desc"

    cursor  = self.__cursor
    cursor.execute(query,(args.sqlPattern, args.syshost, queue, start_date, end_date))
    resultA = cursor.fetchall()

    libA = self.__libA
    for corehours, n_users, n_accts, n_runs, n_jobs, object_path, module in resultA:
      entryT = { 'corehours'   : corehours,
                 'n_users'     : n_users,
                 'n_accts'     : n_accts,
                 'n_runs'      : n_runs,
                 'n_jobs'      : n_jobs,
                 'object_path' : object_path,
                 'module'      : module
               }
      libA.append(entryT)
      

    #q2 = "select ROUND(SUM(t3.num_cores*t3.run_time/3600.0)) as corehours, COUNT(DISTINCT(t3.user)) as n_users, COUNT(t3.date) as n_runs, COUNT(DISTINCT(t3.job_id)) as n_jobs, t1.object_path from xalt_object as t1, join_run_object as t2, xalt_run as t3  where t1.module_name is NULL and t1.obj_id = t2.obj_id and t2.run_id = t3.run_id and t3.date >= '2016-04-04' and t3.date < '2016-04-05' group by t1.object_path order by corehours desc"

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(['CoreHrs', '# Users', '# Accts','# Runs','# Jobs','Library Module'])
    resultA.append(['-------', '-------', '-------','------','------','--------------'])

    libA = self.__libA
    libT = {}

    for entryT in libA:
      module = entryT['module']
      if (module in libT):
        if (entryT['corehours'] > libT[module]['corehours']):
          libT[module] = entryT
      else:
        libT[module] = entryT
    
        
    for k, entryT in sorted(libT.items(), key=lambda v: v[1][sort_key], reverse=True):
      resultA.append(['{:,.0f}'.format(entryT['corehours']), '{:,}'.format(entryT['n_users']), '{:,}'.format(entryT['n_accts']),
                      '{:,}'.format(entryT['n_runs']), '{:,}'.format(entryT['n_jobs']), entryT['module']])

    return resultA
  def group_report_by(self, args, sort_key):
    resultA = []
    resultA.append(['CoreHrs', '# Users', '# Accts', '# Runs', '# Jobs', 'Library Module'])
    resultA.append(['-------', '-------', '-------', '------', '------', '--------------'])

    libA = self.__libA
    libT = {}

    for entryT in libA:
      module = entryT['module']
      if (module in libT):
        if (entryT['corehours'] > libT[module]['corehours']):
          libT[module] = entryT
      else:
        libT[module] = entryT
    
    groupT = {}
    for module in libT:
      sn = shortName(module)
      if (not sn in groupT):
        groupT[sn]           = libT[module]
        groupT[sn]['module'] = sn
      else:
        g_entry = groupT[sn]
        entry   = libT[module]
        for key in g_entry:
          if (key != "module"):
            g_entry[key] += entry[key]
        
    for k, entryT in sorted(groupT.items(), key=lambda v: v[1][sort_key], reverse=True):
      resultA.append(['{:,.0f}'.format(entryT['corehours']), '{:,}'.format(entryT['n_users']) , '{:,}'.format(entryT['n_accts']), '{:,}'.format(entryT['n_runs']), \
                      '{:,}'.format(entryT['n_jobs']), entryT['module']])

    return resultA

        
def percent_str(entry, total):
  result = 0.0
  if (total != 0.0):
    result = '{:,.0f}'.format(100.0 * entry/total)
  return result


def main():
  XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR","./")
  args         = CmdLineOptions().execute()
  config       = configparser.ConfigParser()     
  configFn     = os.path.join(XALT_ETC_DIR,args.confFn)
  config.read(configFn)

  dbName       = config.get("MYSQL","DB")

  syshost      = args.syshost
  if (syshost == '%') :
    syshost    = dbName[5:]  # strip off "xalt_"

  conn = MySQLdb.connect              \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")).decode(), \
          dbName)
  cursor = conn.cursor()

  end_date = time.strftime('%Y-%m-%d')
  if (args.endD is not None):
    end_date = args.endD
  
  start_date = (datetime.strptime(end_date, "%Y-%m-%d") - timedelta(90)).strftime('%Y-%m-%d');
  if (args.startD is not None):
    start_date = args.startD

  print("-------------------------------------------------------------")
  print("XALT Libary Report on",syshost,"from",start_date,"to",end_date)
  print("-------------------------------------------------------------")
  print("")
  print("")

  ############################################################
  #  Report of Library by short module name usage by Core Hours.
  libA = Libraries(cursor)
  libA.build(args, start_date, end_date)
  resultA = libA.group_report_by(args,"corehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
  print("")
  print("---------------------------------------------------------------------------------")
  print("Libraries used by MPI Executables sorted by Core Hours grouped by module families")
  print("---------------------------------------------------------------------------------")
  print("")
  print(bt.build_tbl())
  
  ############################################################
  #  Report of Library usage by Core Hours.
  resultA = libA.report_by(args,"corehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
  print("")
  print("------------------------------------------------------")
  print("Libraries used by MPI Executables sorted by Core Hours")
  print("------------------------------------------------------")
  print("")
  print(bt.build_tbl())
  
if ( __name__ == '__main__'): main()
