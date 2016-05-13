
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
    parser.add_argument("--confFn",  dest='confFn',    action="store",       default = "xalt_db.conf", help="db name")
    parser.add_argument("--start",   dest='startD',    action="store",       default = None,           help="start date")
    parser.add_argument("--end",     dest='endD',      action="store",       default = None,           help="end date")
    parser.add_argument("--syshost", dest='syshost',   action="store",       default = "%",            help="syshost")
    parser.add_argument("--num",     dest='num',       action="store",       default = 20,             help="top number of entries to report")
    args = parser.parse_args()
    return args



class ExecRun:
  """ Holds the executables for a given date range """
  def __init__(self, cursor):
    self.__execA  = []
    self.__cursor = cursor

  def build(self, args, startdate, enddate):
    equiv_patternA = name_mapping()
    sA = []
    sA.append("SELECT CASE ")
    for entry in equiv_patternA:
      left  = entry[0].lower()
      right = entry[1]
      s     = "WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP '%s' then '%s' " % (left, right)
      sA.append(s)

    sA.append(" ELSE SUBSTRING_INDEX(xalt_run.exec_path,'/',-1) END ")
    sA.append(" AS execname, ROUND(SUM(run_time*num_cores/3600)) as totalcput, ")
    sA.append(" COUNT(date) as n_jobs, COUNT(DISTINCT(user)) as n_users ")
    sA.append("   FROM xalt_run ")
    sA.append("  WHERE syshost like '%s' ")
    sA.append("    AND date >= '%s' AND date < '%s' ")
    sA.append("  GROUP BY execname ORDER BY totalcput DESC")

    query  = "".join(sA) % (args.syshost, startdate, enddate)
    cursor = self.__cursor

    cursor.execute(query)
    resultA = cursor.fetchall()

    execA = self.__execA
    for execname, corehours, n_jobs, n_users in resultA:
      entryT = {'execname'  : execname,
                'corehours' : corehours,
                'n_jobs'    : n_jobs,
                'n_users'   : n_users}
      execA.append(entryT)

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["CoreHrs", "# Jobs","# Users", "Exec"])
    resultA.append(["-------", "------","-------", "----"])

    execA = self.__execA
    sortA = sorted(execA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    sumCH = 0.0
    for i in range(num):
      entryT = sortA[i]
      resultA.append(["%.0f" % (entryT['corehours']),  "%d" % (entryT['n_jobs']) , "%d" %(entryT['n_users']), entryT['execname']])
      sumCH += entryT['corehours']
    
    return resultA, sumCH


class Libraries:

  def __init__(self, cursor):
    self.__libA  = []
    self.__cursor = cursor

  def build(self, args, startdate, enddate):
    
    query = "select ROUND(SUM(t3.num_cores*t3.run_time/3600.0)) as corehours,       \
                    COUNT(DISTINCT(t3.user)) as n_users,                            \
                    COUNT(t3.date) as n_runs, COUNT(DISTINCT(t3.job_id)) as n_jobs, \
                    t1.object_path, t1.module_name as module                        \
                    from xalt_object as t1, join_run_object as t2, xalt_run as t3   \
                    where ( t1.module_name is not NULL and t1.module_name != 'NULL')\
                    and t1.obj_id = t2.obj_id and t2.run_id = t3.run_id             \
                    and t3.syshost like %s                                          \
                    and t3.date >= %s and t3.date < %s                              \
                    group by t1.object_path order by corehours desc"

    cursor  = self.__cursor
    cursor.execute(query,(args.syshost, startdate, enddate))
    resultA = cursor.fetchall()

    libA = self.__libA
    for corehours, n_users, n_runs, n_jobs, object_path, module in resultA:
      entryT = { 'corehours'   : corehours,
                 'n_users'     : n_users,
                 'n_runs'      : n_runs,
                 'n_jobs'      : n_jobs,
                 'object_path' : object_path,
                 'module'      : module
               }
      libA.append(entryT)
      

    #q2 = "select ROUND(SUM(t3.num_cores*t3.run_time/3600.0)) as corehours, COUNT(DISTINCT(t3.user)) as n_users, COUNT(t3.date) as n_runs, COUNT(DISTINCT(t3.job_id)) as n_jobs, t1.object_path from xalt_object as t1, join_run_object as t2, xalt_run as t3  where t1.module_name is NULL and t1.obj_id = t2.obj_id and t2.run_id = t3.run_id and t3.date >= '2016-04-04' and t3.date < '2016-04-05' group by t1.object_path order by corehours desc"

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(['CoreHrs', '# Users','# Runs','# Jobs','Library Module'])
    resultA.append(['-------', '-------','------','------','--------------'])

    libA = self.__libA
    libT = {}

    for entryT in libA:
      module = entryT['module']
      if (module in libT):
        if (entryT['corehours'] > libT[module]['corehours']):
          libT[module] = entryT
      else:
        libT[module] = entryT
    
        
    for k, entryT in sorted(libT.iteritems(), key=lambda(k,v): v[sort_key], reverse=True):
      resultA.append(["%.0f" % (entryT['corehours']), "%d" % (entryT['n_users']), "%d" % (entryT['n_runs']), \
                      "%d" % (entryT['n_jobs']), entryT['module']])

    return resultA

  def group_report_by(self, args, sort_key):
    resultA = []
    resultA.append(['CoreHrs', '# Users','# Runs','# Jobs','Library Module'])
    resultA.append(['-------', '-------','------','------','--------------'])

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
    for module in libT
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
        
    for k, entryT in sorted(groupT.iteritems(), key=lambda(k,v): v[sort_key], reverse=True):
      resultA.append(["%.0f" % (entryT['corehours']), "%d" % (entryT['n_users']), "%d" % (entryT['n_runs']), \
                      "%d" % (entryT['n_jobs']), entryT['module']])

    return resultA

def kinds_of_jobs(cursor, args, startdate, enddate):

  query = "SELECT ROUND(SUM(run_time*num_cores/3600)) as corehours,                \
                  COUNT(*) as n_runs, COUNT(DISTINCT(job_id)) as n_jobs,           \
                  COUNT(DISTINCT(user)) as n_users                                 \
                  from xalt_run where syshost like %s                              \
                  and date >= %s and date < %s and exec_type = %s "



  execKindA = [ ["sys",        "binary", "and module_name is not NULL"],
                ["usr"  ,      "binary", "and module_name is NULL"    ],
                ["sys-script", "script", "and module_name is not NULL"],
                ["usr-script", "script", "and module_name is NULL"    ]
              ]

  resultT = {}

  
  totalT  = {'corehours' : 0.0,
             'n_jobs'    : 0.0,
             'n_runs'    : 0.0}

  for entryA in execKindA:
    name   = entryA[0]
    kind   = entryA[1]
    q2     = query + entryA[2]
    cursor.execute(q2,(args.syshost, startdate, enddate, kind))

    if (cursor.rowcount == 0):
      print("Unable to get the number of", kind," jobs: Quitting!")
      sys.exit(1)
    
    row = cursor.fetchall()[0]
    core_hours = row[0]
    if (core_hours == None):
      core_hours = 0.0
    resultT[name] = {'corehours' : float(core_hours),
                     'n_runs'    : int(row[1]),
                     'n_jobs'    : int(row[2]),
                     'n_users'   : int(row[3]),
                     'name'      : name
                     }

    totalT['corehours'] += float(core_hours)
    totalT['n_runs'   ] += int(row[1])
    totalT['n_jobs'   ] += int(row[2])


  resultA = []
  resultA.append(["Kind", "CoreHrs", "   ", "Runs", "   ", "Jobs", "   ", "Users"])
  resultA.append(["    ", "  N    ", " % ", " N  ", " % ", " N  ", " % ", "  N  "])
  resultA.append(["----", "-------", "---", "----", "---", "----", "---", "-----"])
  

     
  for k, entryT in sorted(resultT.iteritems(), key=lambda(k,v): v['corehours'], reverse=True):
    pSU = "%.0f" % (100.0 * entryT['corehours']/totalT['corehours'])
    pR  = "%.0f" % (100.0 * entryT['n_runs']   /float(totalT['n_runs']))
    pJ  = "%.0f" % (100.0 * entryT['n_jobs']   /float(totalT['n_jobs']))

    resultA.append([k,
      "%.0f" % (entryT['corehours']), pSU,
      entryT['n_runs'], pR,
      entryT['n_jobs'], pJ,
      entryT['n_users']])
                 

  resultA.append(["----", "-------", "---", "----", "---", "----", "---", " "])
  resultA.append(["Total", "%.0f" % (totalT['corehours']), "100", "%.0f" % (totalT['n_runs']), "100", "%.0f" % (totalT['n_jobs']), "100", " "])
  return resultA

def running_other_exec(cursor, args, startdate, enddate):

  query = "SELECT ROUND(SUM(t1.num_cores*t1.run_time/3600.0)) as corehours, \
           COUNT(t1.date) as n_runs,                                 \
           COUNT(DISTINCT(t1.user)) as n_users                       \
           FROM xalt_run AS t1, xalt_link AS t2                      \
           WHERE t1.uuid is not NULL and t1.uuid = t2.uuid           \
           and t1.syshost like %s                                    \
           and t1.user != t2.build_user and t1.module_name is NULL   \
           and t1.date >= %s and t1.date < %s"

  resultT = {}
  cursor.execute(query, (args.syshost, startdate, enddate));
  if (cursor.rowcount == 0):
    print("Unable to get the number of user != build_user: Quitting!")
    sys.exit(1)
    
  row = cursor.fetchall()[0]
  
  resultT['diff'] = {'corehours' : float(row[0]),
                     'n_runs'    : int(row[1]),
                     'n_users'   : int(row[2])
                    }

  query = "SELECT ROUND(SUM(t1.num_cores*t1.run_time/3600.0)) as corehours, \
           COUNT(t1.date) as n_runs,                                 \
           COUNT(DISTINCT(t1.user)) as n_users                       \
           FROM xalt_run AS t1, xalt_link AS t2                      \
           WHERE t1.uuid is not NULL and t1.uuid = t2.uuid           \
           and t1.syshost like %s                                    \
           and t1.user = t2.build_user                               \
           and t1.date >= %s and t1.date < %s"
  
  cursor.execute(query, (args.syshost, startdate, enddate));
  if (cursor.rowcount == 0):
    print("Unable to get the number of user != build_user: Quitting!")
    sys.exit(1)
    
  row = cursor.fetchall()[0]
  core_hours = row[0]
  if (core_hours == None):
    core_hours = 0.0
  resultT['same'] = {'corehours' : float(core_hours),
                     'n_runs'    : int(row[1]),
                     'n_users'   : int(row[2])
                    }

  resultA = []
  resultA.append(["Kind", "CoreHrs", "# Runs", "# Users"])
  resultA.append(["----", "-------", "------", "-------"])

  resultA.append(["diff user","%.0f" % (resultT['diff']['corehours']), resultT['diff']['n_runs'], resultT['diff']['n_users']])
  resultA.append(["same user","%.0f" % (resultT['same']['corehours']), resultT['same']['n_runs'], resultT['same']['n_users']])

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

  ############################################################
  #  Over all job counts
  resultA = kinds_of_jobs(cursor, args, startdate, enddate)
  bt      = BeautifulTbl(tbl=resultA, gap = 4, justify = "lrrrrrrr")
  print("----------------------")
  print("Overall MPI Job Counts")
  print("----------------------")
  print("")
  print(bt.build_tbl())
  print("\n")
  print("Where        usr: executables built by user")
  print("             sys: executables managed by system-level modulefiles")
  print("      usr-script: shell scripts in a user's account")
  print("      sys-script: shell scripts managed by system-level modulefiles")

  ############################################################
  #  Self-build vs. BuildU != RunU
  resultA = running_other_exec(cursor, args, startdate, enddate)
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "lrrr")
  print("")
  print("---------------------------------------------------")
  print("Comparing MPI Self-build vs. Build User != Run User")
  print("---------------------------------------------------")
  print("")
  print(bt.build_tbl())

  print("")
  print("-------------------")
  print("Top MPI Executables")
  print("-------------------")
  print("")

  ############################################################
  #  Build top executable list
  execA = ExecRun(cursor)
  execA.build(args, startdate, enddate)
  
  ############################################################
  #  Report of Top EXEC by Core Hours
  resultA, sumCH = execA.report_by(args,"corehours")
  bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nTop",args.num, "MPI Executables sorted by Core-hours (Total Core Hours(M):",sumCH*1.0e-6,")\n")
  print(bt.build_tbl())

  ############################################################
  #  Report of Top EXEC by Num Jobs
  resultA, sumCH  = execA.report_by(args,"n_jobs")
  bt              = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nTop",args.num, "MPI Executables sorted by # Jobs\n")
  print(bt.build_tbl())

  ############################################################
  #  Report of Top EXEC by Users
  resultA, sumCH = execA.report_by(args,"n_users")
  bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nTop",args.num, "MPI Executables sorted by # Users\n")
  print(bt.build_tbl())
  
  ############################################################
  #  Report of Library by short module name usage by Core Hours.
  libA = Libraries(cursor)
  libA.build(args, startdate, enddate)
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
  libA = Libraries(cursor)
  libA.build(args, startdate, enddate)
  resultA = libA.report_by(args,"corehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
  print("")
  print("------------------------------------------------------")
  print("Libraries used by MPI Executables sorted by Core Hours")
  print("------------------------------------------------------")
  print("")
  print(bt.build_tbl())


  


if ( __name__ == '__main__'): main()


