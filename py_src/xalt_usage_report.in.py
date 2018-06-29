
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
    parser.add_argument("--full",    dest='full',      action="store_true",                            help="report node hours by compiler")
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
    sA.append(" AS execname, ROUND(SUM(run_time*num_nodes/3600)) as totalcput, ")
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
    for execname, nodehours, n_jobs, n_users in resultA:
      entryT = {'execname'  : execname,
                'nodehours' : nodehours,
                'n_jobs'    : n_jobs,
                'n_users'   : n_users}
      execA.append(entryT)

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["NodeHrs", "# Jobs","# Users", "Exec"])
    resultA.append(["-------", "------","-------", "----"])

    execA = self.__execA
    sortA = sorted(execA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    sumNH = 0.0
    for i in range(num):
      entryT = sortA[i]
      resultA.append(["%.0f" % (entryT['nodehours']),  "%d" % (entryT['n_jobs']) , "%d" %(entryT['n_users']), entryT['execname']])
      sumNH += entryT['nodehours']
    
    return resultA, sumNH


class ExecRunLink:
  """ Holds the executables for a given date range """
  def __init__(self, cursor):
    self.__execA  = []
    self.__cursor = cursor

  def build(self, args, startdate, enddate, compiler):
    equiv_patternA = name_mapping()
    sA = []
    sA.append("SELECT CASE ")
    for entry in equiv_patternA:
      left  = entry[0].lower()
      right = entry[1]
      s     = "WHEN LOWER(SUBSTRING_INDEX(t1.exec_path,'/',-1)) REGEXP '%s' then '%s' " % (left, right)
      sA.append(s)

    sA.append(" ELSE SUBSTRING_INDEX(t1.exec_path,'/',-1) END ")
    sA.append(" AS execname, ROUND(SUM(t1.run_time*t1.num_nodes/3600)) as totalcput, ")
    sA.append(" COUNT(t1.date) as n_jobs, COUNT(DISTINCT(t1.user)) as n_users")
    sA.append("   FROM xalt_run as t1, xalt_link as t2 ")
    sA.append("  WHERE t1.syshost like '%s' ")
    sA.append("    AND t1.date >= '%s' AND t1.date < '%s' ")
    sA.append("    AND t1.uuid = t2.uuid")
    sA.append("    AND t2.link_program = '%s' ")
    sA.append("  GROUP BY execname ORDER BY totalcput DESC")

    query  = "".join(sA) % (args.syshost, startdate, enddate, compiler)
    cursor = self.__cursor

    cursor.execute(query)
    resultA = cursor.fetchall()

    execA = self.__execA
    for execname, nodehours, n_jobs, n_users in resultA:
      entryT = {'execname'  : execname,
                'nodehours' : nodehours,
                'n_jobs'    : n_jobs,
                'n_users'   : n_users}
      execA.append(entryT)

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["NodeHrs", "# Jobs","# Users", "Exec"])
    resultA.append(["-------", "------","-------", "----"])

    execA = self.__execA
    sortA = sorted(execA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    sumNH = 0.0
    for i in range(num):
      entryT = sortA[i]
      resultA.append(["%.0f" % (entryT['nodehours']),  "%d" % (entryT['n_jobs']) , "%d" %(entryT['n_users']), entryT['execname']])
      sumNH += entryT['nodehours']
    
    return resultA, sumNH


class CompilerUsageByCount:
  def __init__(self, cursor):
    self.__linkA  = []
    self.__cursor = cursor
  def build(self, args, startdate, enddate):
    query = """
    SELECT link_program, count(date) as count FROM xalt_link
    WHERE build_syshost like %s
    AND   date >= %s AND date < %s
    GROUP by link_program
    """
    cursor  = self.__cursor
    cursor.execute(query, (args.syshost, startdate, enddate))
    resultA = cursor.fetchall()
    linkA   = self.__linkA
    for link_program, count in resultA:
      entryT = { 'count'        : count,
                 'link_program' : link_program }
      linkA.append(entryT)
    
  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["Count", "Link Program"])
    resultA.append(["-----", "------------"])

    linkA = self.__linkA
    sortA = sorted(linkA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    for i in range(num):
      entryT = sortA[i]
      resultA.append(["%d" % (entryT['count']), entryT['link_program']])
    
    return resultA

class CompilerUsageByNodeHours:
  def __init__(self, cursor):
    self.__linkA  = []
    self.__cursor = cursor

  def build(self, args, startdate, enddate):
    query = """
    SELECT
    ROUND(SUM(t1.run_time*t1.num_nodes/3600.0)) as nodehours,
    COUNT(t1.date)                              as n_runs,
    COUNT(DISTINCT(t1.user))                    as n_users,
    t2.link_program                             as link_program
    FROM xalt_run as t1, xalt_link as t2
    WHERE t1.uuid is not NULL
    AND   t1.uuid = t2.uuid
    and   t1.syshost like %s
    AND   t1.date >= %s and t1.date < %s
    GROUP by link_program
    """
    cursor  = self.__cursor
    cursor.execute(query, (args.syshost, startdate, enddate))
    resultA = cursor.fetchall()
    linkA   = self.__linkA
    for nodehours, n_runs, n_users, link_program in resultA:
      entryT = { 'nodehours'   : nodehours,
                 'n_users'     : n_users,
                 'n_runs'      : n_runs,
                 'link_program': link_program
               }
      linkA.append(entryT)
    

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(['NodeHrs', '# Users','# Runs','Link Program'])
    resultA.append(['-------', '-------','------','------------'])

    linkA = self.__linkA
    sortA = sorted(linkA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    for i in range(num):
      entryT = sortA[i]
      resultA.append(["%.0f" % (entryT['nodehours']), "%d" % (entryT['n_users']), "%d" % (entryT['n_runs']), \
                      entryT['link_program']])
    return resultA

class Libraries:

  def __init__(self, cursor):
    self.__libA  = []
    self.__cursor = cursor

  def build(self, args, startdate, enddate, queue="%"):
    
    query = "select ROUND(SUM(t3.num_nodes*t3.run_time/3600.0)) as nodehours,        \
                    COUNT(DISTINCT(t3.user)) as n_users,                             \
                    COUNT(t3.date) as n_runs, COUNT(DISTINCT(t3.job_id)) as n_jobs,  \
                    t1.object_path, t1.module_name as module                         \
                    from xalt_object as t1, join_run_object as t2, xalt_run as t3    \
                    where ( t1.module_name is not NULL and t1.module_name != 'NULL') \
                    and t1.obj_id = t2.obj_id and t2.run_id = t3.run_id              \
                    and t3.syshost like %s and t3.queue like %s                      \
                    and t3.date >= %s and t3.date < %s                               \
                    group by t1.object_path order by nodehours desc"

    cursor  = self.__cursor
    cursor.execute(query,(args.syshost, queue, startdate, enddate))
    resultA = cursor.fetchall()

    libA = self.__libA
    for nodehours, n_users, n_runs, n_jobs, object_path, module in resultA:
      entryT = { 'nodehours'   : nodehours,
                 'n_users'     : n_users,
                 'n_runs'      : n_runs,
                 'n_jobs'      : n_jobs,
                 'object_path' : object_path,
                 'module'      : module
               }
      libA.append(entryT)
      

    #q2 = "select ROUND(SUM(t3.num_nodes*t3.run_time/3600.0)) as nodehours, COUNT(DISTINCT(t3.user)) as n_users, COUNT(t3.date) as n_runs, COUNT(DISTINCT(t3.job_id)) as n_jobs, t1.object_path from xalt_object as t1, join_run_object as t2, xalt_run as t3  where t1.module_name is NULL and t1.obj_id = t2.obj_id and t2.run_id = t3.run_id and t3.date >= '2016-04-04' and t3.date < '2016-04-05' group by t1.object_path order by nodehours desc"

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(['NodeHrs', '# Users','# Runs','# Jobs','Library Module'])
    resultA.append(['-------', '-------','------','------','--------------'])

    libA = self.__libA
    libT = {}

    for entryT in libA:
      module = entryT['module']
      if (module in libT):
        if (entryT['nodehours'] > libT[module]['nodehours']):
          libT[module] = entryT
      else:
        libT[module] = entryT
    
        
    for k, entryT in sorted(libT.iteritems(), key=lambda(k,v): v[sort_key], reverse=True):
      resultA.append(["%.0f" % (entryT['nodehours']), "%d" % (entryT['n_users']), "%d" % (entryT['n_runs']), \
                      "%d" % (entryT['n_jobs']), entryT['module']])

    return resultA
  def group_report_by(self, args, sort_key):
    resultA = []
    resultA.append(['NodeHrs', '# Users','# Runs','# Jobs','Library Module'])
    resultA.append(['-------', '-------','------','------','--------------'])

    libA = self.__libA
    libT = {}

    for entryT in libA:
      module = entryT['module']
      if (module in libT):
        if (entryT['nodehours'] > libT[module]['nodehours']):
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
        
    for k, entryT in sorted(groupT.iteritems(), key=lambda(k,v): v[sort_key], reverse=True):
      resultA.append(["%.0f" % (entryT['nodehours']), "%d" % (entryT['n_users']), "%d" % (entryT['n_runs']), \
                      "%d" % (entryT['n_jobs']), entryT['module']])

    return resultA

class ModuleExec:
  def __init__(self, cursor):
    self.__modA  = []
    self.__cursor = cursor

  def build(self, args, startdate, enddate):
    query = """
    SELECT 
    ROUND(SUM(run_time*num_nodes/3600)) as nodehours,
    count(date)                         as n_jobs,
    COUNT(DISTINCT(user))               as n_users,
    module_name                         as modules
    from xalt_run where syshost like %s
    and date >= %s and date < %s and  module_name is not null
    group by modules
    """
    cursor  = self.__cursor
    cursor.execute(query, (args.syshost, startdate, enddate))
    resultA = cursor.fetchall()
    modA   = self.__modA
    for nodehours, n_jobs, n_users, modules in resultA:
      entryT = { 'nodehours' : nodehours,
                 'n_jobs'    : n_jobs,
                 'n_users'   : n_users,
                 'modules'   : modules }
      modA.append(entryT)

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["NodeHrs", "# Jobs","# Users", "Modules"])
    resultA.append(["-------", "------","-------", "-------"])

    modA = self.__modA
    sortA = sorted(modA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    for i in range(num):
      entryT = sortA[i]
      resultA.append(["%.0f" % (entryT['nodehours']),  "%d" % (entryT['n_jobs']) , "%d" %(entryT['n_users']), entryT['modules']])
    
    return resultA
        
def kinds_of_jobs(cursor, args, startdate, enddate):

  query = "SELECT ROUND(SUM(run_time*num_nodes/3600)) as nodehours,                \
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

  
  totalT  = {'nodehours' : 0.0,
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
    node_hours = float(row[0] or 0.0)
    resultT[name] = {'nodehours' : node_hours,
                     'n_runs'    : int(row[1]),
                     'n_jobs'    : int(row[2]),
                     'n_users'   : int(row[3]),
                     'name'      : name
                     }

    totalT['nodehours'] += node_hours
    totalT['n_runs'   ] += int(row[1])
    totalT['n_jobs'   ] += int(row[2])


  resultA = []
  resultA.append(["Kind", "NodeHrs", "   ", "Runs", "   ", "Jobs", "   ", "Users"])
  resultA.append(["    ", "  N    ", " % ", " N  ", " % ", " N  ", " % ", "  N  "])
  resultA.append(["----", "-------", "---", "----", "---", "----", "---", "-----"])
  

     
  for k, entryT in sorted(resultT.iteritems(), key=lambda(k,v): v['nodehours'], reverse=True):
    pSU = "%.0f" % (100.0 * entryT['nodehours']/totalT['nodehours'])
    pR  = "%.0f" % (100.0 * entryT['n_runs']   /float(totalT['n_runs']))
    pJ  = "%.0f" % (100.0 * entryT['n_jobs']   /float(totalT['n_jobs']))

    resultA.append([k,
      "%.0f" % (entryT['nodehours']), pSU,
      entryT['n_runs'], pR,
      entryT['n_jobs'], pJ,
      entryT['n_users']])
                 

  resultA.append(["----", "-------", "---", "----", "---", "----", "---", " "])
  resultA.append(["Total", "%.0f" % (totalT['nodehours']), "100", "%.0f" % (totalT['n_runs']), "100", "%.0f" % (totalT['n_jobs']), "100", " "])
  return resultA

def running_other_exec(cursor, args, startdate, enddate):

  query = "SELECT ROUND(SUM(t1.num_nodes*t1.run_time/3600.0)) as nodehours, \
           COUNT(t1.date)                                     as n_runs,    \
           COUNT(DISTINCT(t1.user))                           as n_users    \
           FROM xalt_run AS t1, xalt_link AS t2                             \
           WHERE t1.uuid is not NULL and t1.uuid = t2.uuid                  \
           and t1.syshost like %s                                           \
           and t1.user != t2.build_user and t1.module_name is NULL          \
           and t1.date >= %s and t1.date < %s"

  resultT = {}
  cursor.execute(query, (args.syshost, startdate, enddate));
  if (cursor.rowcount == 0):
    print("Unable to get the number of user != build_user: Quitting!")
    sys.exit(1)
    
  row = cursor.fetchall()[0]
  
  node_hours = float(row[0] or 0.0)

  resultT['diff'] = {'nodehours' : node_hours,
                     'n_runs'    : int(row[1]),
                     'n_users'   : int(row[2])
                    }

  query = "SELECT ROUND(SUM(t1.num_nodes*t1.run_time/3600.0)) as nodehours, \
           COUNT(t1.date)                                     as n_runs,    \
           COUNT(DISTINCT(t1.user))                           as n_users    \
           FROM xalt_run AS t1, xalt_link AS t2                             \
           WHERE t1.uuid is not NULL and t1.uuid = t2.uuid                  \
           and t1.syshost like %s                                           \
           and t1.user = t2.build_user                                      \
           and t1.date >= %s and t1.date < %s"
  
  cursor.execute(query, (args.syshost, startdate, enddate));
  if (cursor.rowcount == 0):
    print("Unable to get the number of user != build_user: Quitting!")
    sys.exit(1)
    
  row = cursor.fetchall()[0]
  resultT['same'] = {'nodehours' : float(row[0] or 0.0),
                     'n_runs'    : int(row[1]),
                     'n_users'   : int(row[2])
                    }

  resultA = []
  resultA.append(["Kind", "NodeHrs", "# Runs", "# Users"])
  resultA.append(["----", "-------", "------", "-------"])

  resultA.append(["diff user","%.0f" % (resultT['diff']['nodehours']), resultT['diff']['n_runs'], resultT['diff']['n_users']])
  resultA.append(["same user","%.0f" % (resultT['same']['nodehours']), resultT['same']['n_runs'], resultT['same']['n_users']])

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
  print("XALT REPORT from",startdate,"to",enddate)
  print("--------------------------------------------")
  print("")
  print("")
  
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
  #  Report of Top EXEC by Node Hours
  resultA, sumNH = execA.report_by(args,"nodehours")
  bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nTop",args.num, "MPI Executables sorted by Node-hours (Total Node Hours(M):",sumNH*1.0e-6,")\n")
  print(bt.build_tbl())
  
  ############################################################
  #  Report of Top EXEC by Num Jobs
  resultA, sumNH  = execA.report_by(args,"n_jobs")
  bt              = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nTop",args.num, "MPI Executables sorted by # Jobs\n")
  print(bt.build_tbl())
  
  ############################################################
  #  Report of Top EXEC by Users
  resultA, sumNH = execA.report_by(args,"n_users")
  bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nTop",args.num, "MPI Executables sorted by # Users\n")
  print(bt.build_tbl())
  
  if (args.full):
    ############################################################
    #  Report of Top EXEC by Nodehours for gcc
    execA          = ExecRunLink(cursor)
    execA.build(args, startdate, enddate,"gcc")
    resultA, sumNH = execA.report_by(args,"nodehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Node-hours for gcc\n")
    print(bt.build_tbl())

    ############################################################
    #  Report of Top EXEC by Nodehours for g++
    execA          = ExecRunLink(cursor)
    execA.build(args, startdate, enddate,"g++")
    resultA, sumNH = execA.report_by(args,"nodehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Node-hours for g++\n")
    print(bt.build_tbl())
  
    ############################################################
    #  Report of Top EXEC by Nodehours for gfortran
    execA          = ExecRunLink(cursor)
    execA.build(args, startdate, enddate,"gfortran")
    resultA, sumNH = execA.report_by(args,"nodehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Node-hours for gfortran\n")
    print(bt.build_tbl())

    ############################################################
    #  Report of Top EXEC by Nodehours for ifort
    execA          = ExecRunLink(cursor)
    execA.build(args, startdate, enddate,"ifort")
    resultA, sumNH = execA.report_by(args,"nodehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Node-hours for ifort\n")
    print(bt.build_tbl())

    ############################################################
    #  Report of Top EXEC by Nodehours for icc
    execA          = ExecRunLink(cursor)
    execA.build(args, startdate, enddate,"icc")
    resultA, sumNH = execA.report_by(args,"nodehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Node-hours for icc\n")
    print(bt.build_tbl())
  
    ############################################################
    #  Report of Top EXEC by Nodehours for icpc
    execA          = ExecRunLink(cursor)
    execA.build(args, startdate, enddate,"icpc")
    resultA, sumNH = execA.report_by(args,"nodehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Node-hours for icpc\n")
    print(bt.build_tbl())

  ############################################################
  #  Report of Top Modules by Node Hours
  modA = ModuleExec(cursor)
  modA.build(args, startdate, enddate)
  resultA = modA.report_by(args,"nodehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nTop",args.num, "MPI Modules sorted by Node-hours \n")
  print(bt.build_tbl())
  
  ############################################################
  # Report on Compiler (linker) usage by Count
  linkA = CompilerUsageByCount(cursor)
  linkA.build(args, startdate, enddate)
  resultA = linkA.report_by(args, "count")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rl")
  print("\nCompiler usage by Count\n")
  print(bt.build_tbl())
  
  ############################################################
  # Report on Compiler (linker) usage by Node Hours
  linkA = CompilerUsageByNodeHours(cursor)
  linkA.build(args, startdate, enddate)
  resultA = linkA.report_by(args, "nodehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nCompiler usage by Nodehours\n")
  print(bt.build_tbl())
  
  ############################################################
  #  Report of Library by short module name usage by Node Hours.
  libA = Libraries(cursor)
  libA.build(args, startdate, enddate)
  resultA = libA.group_report_by(args,"nodehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
  print("")
  print("---------------------------------------------------------------------------------")
  print("Libraries used by MPI Executables sorted by Node Hours grouped by module families")
  print("---------------------------------------------------------------------------------")
  print("")
  print(bt.build_tbl())
  
  ############################################################
  #  Report of Library usage by Node Hours.
  resultA = libA.report_by(args,"nodehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
  print("")
  print("------------------------------------------------------")
  print("Libraries used by MPI Executables sorted by Node Hours")
  print("------------------------------------------------------")
  print("")
  print(bt.build_tbl())
  
  ############################################################
  #  Report of Library usage by Node Hours for largemem.
  libA = Libraries(cursor)
  libA.build(args, startdate, enddate, "largemem")
  resultA = libA.report_by(args,"nodehours")
  if (resultA):
    bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
    print("")
    print("-------------------------------------------------------------------")
    print("Libraries used by MPI Executables sorted by Node Hours for largemem")
    print("-------------------------------------------------------------------")
    print("")
    print(bt.build_tbl())

if ( __name__ == '__main__'): main()
