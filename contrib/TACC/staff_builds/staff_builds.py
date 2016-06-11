#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, re
import MySQLdb, argparse
import time
from operator import itemgetter
from staff import xalt_tacc_staff
try:
  import configparser
except:
  import ConfigParser as configparser

from BeautifulTbl      import BeautifulTbl

class StaffBuilds:
  def __init__(self, cursor):
    self.__execA  = []
    self.__cursor = cursor

  def build(self, args, startdate, enddate):
    staffA = xalt_tacc_staff()
    sA  = []
    ssA = []
    sA.append("SELECT ROUND(SUM(run_time*num_cores/3600.0)) as corehours, ")
    sA.append("COUNT(date)                                  as n_runs, ")
    sA.append("COUNT(DISTINCT(user))                        as n_users, ")
    sA.append("exec_path ")
    sA.append("FROM  xalt_run ")
    sA.append("WHERE syshost like %s ")
    sA.append("AND   date >= %s AND date < %s ")
    sA.append('AND (')
    for staff in staffA:
      s = "exec_path like '/%%%s%%/'" % (staff)
      ssA.append(s)

    sA.append(" OR ".join(ssA))
    sA.append(" ) ")
    sA.append("GROUP by exec_path order by n_users")

    query  = "".join(sA)
    cursor = self.__cursor
    cursor.execute(query, (args.syshost, startdate, enddate))
    resultA = cursor.fetchall()
    execA = self.__execA
    for corehours, n_runs, n_users, execname in resultA:
      entryT = {'execname'  : execname,
                'corehours' : corehours,
                'n_runs'    : n_runs,
                'n_users'   : n_users}
      execA.append(entryT)

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["CoreHrs", "# Run","# Users", "Exec_Path"])
    resultA.append(["-------", "-----","-------", "---------"])

    execA = self.__execA
    sortA = sorted(execA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    for i in range(num):
      entryT = sortA[i]
      resultA.append(["%.0f" % (entryT['corehours']),  "%d" % (entryT['n_runs']) , "%d" %(entryT['n_users']), entryT['execname']])
    
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

  print("--------------------------------------------")
  print("Staff Builds from",startdate,"to",enddate)
  print("--------------------------------------------")
  print("")

  builds = StaffBuilds(cursor)
  builds.build(args, startdate, enddate)

  resultA = builds.report_by(args, "n_users")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("Staff Builds")
  print(bt.build_tbl())
  

if ( __name__ == '__main__'): main()
