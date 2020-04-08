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
    parser.add_argument("--confFn",  dest='confFn',    action="store",       default = "xalt_db.conf", help="db name")
    parser.add_argument("--start",   dest='startD',    action="store",       default = None,           help="start date")
    parser.add_argument("--end",     dest='endD',      action="store",       default = None,           help="end date")
    parser.add_argument("--syshost", dest='syshost',   action="store",       default = "%",            help="syshost")
    parser.add_argument("--queue",   dest='queue',     action="store",       default = "%",            help="queue")
    parser.add_argument("--num",     dest='num',       action="store",       default = 20,             help="top number of entries to report")
    parser.add_argument("--full",    dest='full',      action="store_true",                            help="report core hours by compiler")
    parser.add_argument("--short",   dest='short',     action="store_true",                            help="Only report on executable, not libraries")
    args = parser.parse_args()
    return args

class ExecRun:
  """ Holds the executables for a given date range """
  def __init__(self, cursor):
    self.__execA  = []
    self.__cursor = cursor

  def build(self, args, style, start_date, end_date):
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
    sA.append(" COUNT(date) as n_runs, COUNT(DISTINCT(user)) as n_users, ",)
    sA.append(" COUNT(DISTINCT(account)) as n_accts ",)
    sA.append("   FROM xalt_run ")
    sA.append("  WHERE syshost like '%s' ")
    sA.append("    AND queue like '%s' ")
    sA.append("    AND date >= '%s' AND date < '%s' ")
    if (style == 'Scalar'):
       sA.append("    AND num_cores <= '1' ")
    elif (style == 'Scalar_05'):
       sA.append("    AND num_cores <= '1' ")
       sA.append("    AND run_time < '300.0' ")
    elif (style == 'Scalar_10'):
       sA.append("    AND num_cores <= '1' ")
       sA.append("    AND run_time < '600.0' ")
    elif (style == 'Scalar_20'):
       sA.append("    AND num_cores <= '1' ")
       sA.append("    AND run_time < '1200.0' ")
    elif (style == 'Scalar_30'):
       sA.append("    AND num_cores <= '1' ")
       sA.append("    AND run_time < '1800.0' ")
    elif (style == 'MPI'):
       sA.append("    AND num_cores > '1' ")
    sA.append("  GROUP BY execname ORDER BY totalcput DESC")

    query  = "".join(sA) % (args.syshost, args.queue, start_date, end_date)
    cursor = self.__cursor

    cursor.execute(query)
    resultA = cursor.fetchall()

    execA = self.__execA
    for execname, corehours, n_runs, n_users, n_accts in resultA:
      entryT = {'execname'  : execname,
                'corehours' : corehours,
                'n_runs'    : n_runs,
                'n_users'   : n_users,
                'n_accts'   : n_accts}
      execA.append(entryT)

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["CoreHrs", "# Runs", "# Users", "# Accts", "Exec"])
    resultA.append(["-------", "------", "-------", "-------", "----"])

    execA = self.__execA
    sortA = sorted(execA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    sumCH = 0.0
    for i in range(num):
      entryT = sortA[i]
      resultA.append(['{:,.0f}'.format(entryT['corehours']),  '{:,}'.format(entryT['n_runs']) , '{:,}'.format(entryT['n_users']),
                      '{:,}'.format(entryT['n_accts']), entryT['execname']])
      sumCH += entryT['corehours']
    
    return resultA, sumCH


class ExecRunLink:
  """ Holds the executables for a given date range """
  def __init__(self, cursor):
    self.__execA  = []
    self.__cursor = cursor

  def build(self, args, start_date, end_date, compiler):
    equiv_patternA = name_mapping()
    sA = []
    sA.append("SELECT CASE ")
    for entry in equiv_patternA:
      left  = entry[0].lower()
      right = entry[1]
      s     = "WHEN LOWER(SUBSTRING_INDEX(t1.exec_path,'/',-1)) REGEXP '%s' then '%s' " % (left, right)
      sA.append(s)

    sA.append(" ELSE SUBSTRING_INDEX(t1.exec_path,'/',-1) END ")
    sA.append(" AS execname, ROUND(SUM(t1.run_time*t1.num_cores/3600)) as totalcput, ")
    sA.append(" COUNT(t1.date) as n_runs, COUNT(DISTINCT(t1.user)) as n_users,")
    sA.append(" COUNT(DISTINCT(t1.account)) as n_accts")
    sA.append("   FROM xalt_run as t1, xalt_link as t2 ")
    sA.append("  WHERE t1.syshost like '%s' ")
    sA.append("    AND t1.date >= '%s' AND t1.date < '%s' ")
    sA.append("    AND t1.uuid = t2.uuid")
    sA.append("    AND t2.link_program = '%s' ")
    sA.append("  GROUP BY execname ORDER BY totalcput DESC")

    query  = "".join(sA) % (args.syshost, start_date, end_date, compiler)
    cursor = self.__cursor

    cursor.execute(query)
    resultA = cursor.fetchall()

    execA = self.__execA
    for execname, corehours, n_runs, n_users, n_accts in resultA:
      entryT = {'execname'  : execname,
                'corehours' : corehours,
                'n_runs'    : n_runs,
                'n_users'   : n_users,
                'n_accts'   : n_accts}
      execA.append(entryT)

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["CoreHrs", "# Runs", "# Users", "# Accts", "Exec"])
    resultA.append(["-------", "------", "-------", "-------", "----"])

    execA = self.__execA
    sortA = sorted(execA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    sumCH = 0.0
    for i in range(num):
      entryT = sortA[i]
      resultA.append(['{:,.0f}'.format(entryT['corehours']),  '{:,}'.format(entryT['n_runs']),
                      '{:,}'.format(entryT['n_users']), '{:,}'.format(entryT['n_accts']),
                      entryT['execname']])
      sumCH += entryT['corehours']
    
    return resultA, sumCH


class CompilerUsageByCount:
  def __init__(self, cursor):
    self.__linkA  = []
    self.__cursor = cursor
  def build(self, args, start_date, end_date):
    query = """
    SELECT link_program, count(date) as count FROM xalt_link
    WHERE build_syshost like %s
    AND   queue like %s  
    AND   date >= %s AND date < %s
    GROUP by link_program
    """
    cursor  = self.__cursor
    cursor.execute(query, (args.syshost, args.queue, start_date, end_date))
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
      resultA.append(['{:,}'.format(entryT['count']), entryT['link_program']])
    
    return resultA

class CompilerUsageByCoreHours:
  def __init__(self, cursor):
    self.__linkA  = []
    self.__cursor = cursor

  def build(self, args, start_date, end_date):
    query = """
    SELECT
    ROUND(SUM(t1.run_time*t1.num_cores/3600.0)) as corehours,
    COUNT(t1.date)                              as n_runs,
    COUNT(DISTINCT(t1.user))                    as n_users,
    COUNT(DISTINCT(t1.account))                 as n_accts,
    t2.link_program                             as link_program
    FROM xalt_run as t1, xalt_link as t2
    WHERE t1.uuid is not NULL
    AND   t1.uuid = t2.uuid
    AND   t1.syshost like %s
    AND   t1.queue   like %s
    AND   t1.date >= %s and t1.date < %s
    GROUP by link_program
    """
    cursor  = self.__cursor
    cursor.execute(query, (args.syshost, args.queue, start_date, end_date))
    resultA = cursor.fetchall()
    linkA   = self.__linkA
    for corehours, n_runs, n_users, n_accts, link_program in resultA:
      entryT = { 'corehours'   : corehours,
                 'n_users'     : n_users,
                 'n_accts'     : n_accts,
                 'n_runs'      : n_runs,
                 'link_program': link_program
               }
      linkA.append(entryT)
    

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(['CoreHrs', '# Users', '# Accts', '# Runs', 'Link Program'])
    resultA.append(['-------', '-------', '-------', '------', '------------'])

    linkA = self.__linkA
    sortA = sorted(linkA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    for i in range(num):
      entryT = sortA[i]
      resultA.append(['{:,.0f}'.format(entryT['corehours']), '{:,}'.format(entryT['n_users']), '{:,}'.format(entryT['n_accts']), '{:,}'.format(entryT['n_runs']), \
                      entryT['link_program']])
    return resultA

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
                    where ( t1.module_name is not NULL and t1.module_name != 'NULL') \
                    and t1.obj_id = t2.obj_id and t2.run_id = t3.run_id              \
                    and t3.syshost like %s and t3.queue like %s                      \
                    and t3.date >= %s and t3.date < %s                               \
                    group by t1.object_path order by corehours desc"

    cursor  = self.__cursor
    cursor.execute(query,(args.syshost, queue, start_date, end_date))
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

class ModuleExec:
  def __init__(self, cursor):
    self.__modA  = []
    self.__cursor = cursor

  def build(self, args, start_date, end_date):
    query = """
    SELECT 
    ROUND(SUM(run_time*num_cores/3600)) as corehours,
    count(date)                         as n_runs,
    COUNT(DISTINCT(user))               as n_users,
    COUNT(DISTINCT(account))            as n_accts,
    module_name                         as modules
    from xalt_run where syshost like %s
    and date >= %s and date < %s and  module_name is not null
    group by modules
    """
    cursor  = self.__cursor
    cursor.execute(query, (args.syshost, start_date, end_date))
    resultA = cursor.fetchall()
    modA   = self.__modA
    for corehours, n_runs, n_users, n_accts,  modules in resultA:
      entryT = { 'corehours' : corehours,
                 'n_runs'    : n_runs,
                 'n_users'   : n_users,
                 'n_accts'   : n_accts,
                 'modules'   : modules }
      modA.append(entryT)

  def report_by(self, args, sort_key):
    resultA = []
    resultA.append(["CoreHrs", "# Runs","# Users","# Accts", "Modules"])
    resultA.append(["-------", "------","-------","-------", "-------"])

    modA = self.__modA
    sortA = sorted(modA, key=itemgetter(sort_key), reverse=True)
    num = min(int(args.num), len(sortA))
    for i in range(num):
      entryT = sortA[i]
      resultA.append(['{:,.0f}'.format(entryT['corehours']),  '{:,}'.format(entryT['n_runs']),
                      '{:,}'.format(entryT['n_users']), '{:,}'.format(entryT['n_accts']), entryT['modules']])
    
    return resultA
        
def kinds_of_jobs(cursor, args, start_date, end_date):

  query = "SELECT ROUND(SUM(run_time*num_cores/3600)) as corehours,                \
                  COUNT(*) as n_runs, COUNT(DISTINCT(job_id)) as n_jobs,           \
                  COUNT(DISTINCT(user)) as n_users,                                \
                  COUNT(DISTINCT(account)) as n_accts                              \
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
    cursor.execute(q2,(args.syshost, start_date, end_date, kind))

    if (cursor.rowcount == 0):
      print("Unable to get the number of", kind," jobs: Quitting!")
      sys.exit(1)
    
    row = cursor.fetchall()[0]
    core_hours = float(row[0] or 0.0)
    resultT[name] = {'corehours' : core_hours,
                     'n_runs'    : int(row[1]),
                     'n_jobs'    : int(row[2]),
                     'n_users'   : int(row[3]),
                     'n_accts'   : int(row[4]),
                     'name'      : name
                     }

    totalT['corehours'] += core_hours
    totalT['n_runs'   ] += int(row[1])
    totalT['n_jobs'   ] += int(row[2])


  resultA = []
  resultA.append(["Kind", "CoreHrs", "   ", "Runs", "   ", "Jobs", "   ", "Users", "Accts"])
  resultA.append(["    ", "  N    ", " % ", " N  ", " % ", " N  ", " % ", "  N  ", "  N  "])
  resultA.append(["----", "-------", "---", "----", "---", "----", "---", "-----", "-----"])
  
  for k, entryT in sorted(resultT.items(), key=lambda v: v[1]['corehours'],reverse=True):
    pSU = percent_str(entryT['corehours'], totalT['corehours'])
    pR  = percent_str(entryT['n_runs'],    float(totalT['n_runs']))
    pJ  = percent_str(entryT['n_jobs'],    float(totalT['n_jobs']))

    resultA.append([k,
      '{:,.0f}'.format(entryT['corehours']), pSU,
      '{:,}'.format(entryT['n_runs']),       pR,
      '{:,}'.format(entryT['n_jobs']),       pJ,
      '{:,}'.format(entryT['n_users']),
      '{:,}'.format(entryT['n_accts']),
    ])
                 

  resultA.append(["----", "-------", "---", "----", "---", "----", "---", " "])
  resultA.append(["Total", '{:,.0f}'.format(totalT['corehours']), "100", '{:,.0f}'.format(totalT['n_runs']), "100", '{:,.0f}'.format(totalT['n_jobs']), "100", " "])
  return resultA

def percent_str(entry, total):
  result = 0.0
  if (total != 0.0):
    result = '{:,.0f}'.format(100.0 * entry/total)
  return result


def running_other_exec(cursor, args, start_date, end_date):

  query = "SELECT ROUND(SUM(t1.num_cores*t1.run_time/3600.0)) as corehours, \
           COUNT(t1.date)                                     as n_runs,    \
           COUNT(DISTINCT(t1.user))                           as n_users    \
           FROM xalt_run AS t1, xalt_link AS t2                             \
           WHERE t1.uuid is not NULL and t1.uuid = t2.uuid                  \
           and t1.syshost like %s                                           \
           and t1.user != t2.build_user and t1.module_name is NULL          \
           and t1.date >= %s and t1.date < %s"

  resultT = {}
  cursor.execute(query, (args.syshost, start_date, end_date));
  if (cursor.rowcount == 0):
    print("Unable to get the number of user != build_user: Quitting!")
    sys.exit(1)
    
  row = cursor.fetchall()[0]
  
  core_hours = float(row[0] or 0.0)

  resultT['diff'] = {'corehours' : core_hours,
                     'n_runs'    : int(row[1]),
                     'n_users'   : int(row[2])
                    }

  query = "SELECT ROUND(SUM(t1.num_cores*t1.run_time/3600.0)) as corehours, \
           COUNT(t1.date)                                     as n_runs,    \
           COUNT(DISTINCT(t1.user))                           as n_users    \
           FROM xalt_run AS t1, xalt_link AS t2                             \
           WHERE t1.uuid is not NULL and t1.uuid = t2.uuid                  \
           and t1.syshost like %s                                           \
           and t1.user = t2.build_user                                      \
           and t1.date >= %s and t1.date < %s"
  
  cursor.execute(query, (args.syshost, start_date, end_date));
  if (cursor.rowcount == 0):
    print("Unable to get the number of user != build_user: Quitting!")
    sys.exit(1)
    
  row = cursor.fetchall()[0]
  resultT['same'] = {'corehours' : float(row[0] or 0.0),
                     'n_runs'    : int(row[1]),
                     'n_users'   : int(row[2])
                    }

  resultA = []
  resultA.append(["Kind", "CoreHrs", "# Runs", "# Users"])
  resultA.append(["----", "-------", "------", "-------"])

  resultA.append(["diff user",'{:,.0f}'.format(resultT['diff']['corehours']), '{:,}'.format(resultT['diff']['n_runs']), '{:,}'.format(resultT['diff']['n_users'])])
  resultA.append(["same user",'{:,.0f}'.format(resultT['same']['corehours']), '{:,}'.format(resultT['same']['n_runs']), '{:,}'.format(resultT['same']['n_users'])])

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
          base64.b64decode(config.get("MYSQL","PASSWD")).decode(), \
          config.get("MYSQL","DB"))
  cursor = conn.cursor()

  end_date = time.strftime('%Y-%m-%d')
  if (args.endD is not None):
    end_date = args.endD
  
  start_date = (datetime.strptime(end_date, "%Y-%m-%d") - timedelta(90)).strftime('%Y-%m-%d');
  if (args.startD is not None):
    start_date = args.startD

  print("--------------------------------------------")
  print("XALT REPORT from",start_date,"to",end_date)
  print("--------------------------------------------")
  print("")
  print("")
  
  ############################################################
  #  Over all job counts
  resultA = kinds_of_jobs(cursor, args, start_date, end_date)
  bt      = BeautifulTbl(tbl=resultA, gap = 4, justify = "lrrrrrrrr")
  print("----------------------")
  print("Overall Job Counts")
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
  resultA = running_other_exec(cursor, args, start_date, end_date)
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "lrrr")
  print("")
  print("-----------------------------------------------")
  print("Comparing Self-build vs. Build User != Run User")
  print("-----------------------------------------------")
  print("")
  print(bt.build_tbl())
  
  for style in ('All','MPI','Non-MPI'):
  
    ############################################################
    #  Build top executable list for style type
    execA = ExecRun(cursor)
    execA.build(args, style, start_date, end_date)
    
    ############################################################
    #  Report of Top EXEC of All types by Core Hours
    resultA, sumCH = execA.report_by(args,"corehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
    print("\nTop",args.num, style+" Executables sorted by Core-hours (Total Core Hours(M):",
            sumCH*1.0e-6,")\n")
    print(bt.build_tbl())
  
    ############################################################
    #  Report of Top EXEC of All types by Num Runs
    resultA, sumCH  = execA.report_by(args,"n_runs")
    bt              = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
    print("\nTop",args.num, style+" Executables sorted by # Runs\n")
    print(bt.build_tbl())
  
    ############################################################
    #  Report of Top EXEC by All types by Users
    resultA, sumCH = execA.report_by(args,"n_users")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
    print("\nTop",args.num, style+" Executables sorted by # Users\n")
    print(bt.build_tbl())
  
  ############################################################
  #  Report of Top Modules by Core Hours
  modA = ModuleExec(cursor)
  modA.build(args, start_date, end_date)
  resultA = modA.report_by(args,"corehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
  print("\nTop",args.num, "Modules used sorted by Core-hours \n")
  print(bt.build_tbl())

  return


  if (args.full):
    ############################################################
    #  Report of Top EXEC by Corehours for gcc
    execA          = ExecRunLink(cursor)
    execA.build(args, start_date, end_date,"gcc")
    resultA, sumCH = execA.report_by(args,"corehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Core-hours for gcc\n")
    print(bt.build_tbl())

    ############################################################
    #  Report of Top EXEC by Corehours for g++
    execA          = ExecRunLink(cursor)
    execA.build(args, start_date, end_date,"g++")
    resultA, sumCH = execA.report_by(args,"corehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Core-hours for g++\n")
    print(bt.build_tbl())
  
    ############################################################
    #  Report of Top EXEC by Corehours for gfortran
    execA          = ExecRunLink(cursor)
    execA.build(args, start_date, end_date,"gfortran")
    resultA, sumCH = execA.report_by(args,"corehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Core-hours for gfortran\n")
    print(bt.build_tbl())

    ############################################################
    #  Report of Top EXEC by Corehours for ifort
    execA          = ExecRunLink(cursor)
    execA.build(args, start_date, end_date,"ifort")
    resultA, sumCH = execA.report_by(args,"corehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Core-hours for ifort\n")
    print(bt.build_tbl())

    ############################################################
    #  Report of Top EXEC by Corehours for icc
    execA          = ExecRunLink(cursor)
    execA.build(args, start_date, end_date,"icc")
    resultA, sumCH = execA.report_by(args,"corehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Core-hours for icc\n")
    print(bt.build_tbl())
  
    ############################################################
    #  Report of Top EXEC by Corehours for icpc
    execA          = ExecRunLink(cursor)
    execA.build(args, start_date, end_date,"icpc")
    resultA, sumCH = execA.report_by(args,"corehours")
    bt             = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
    print("\nTop",args.num, "MPI Executables sorted by Core-hours for icpc\n")
    print(bt.build_tbl())

  
  ############################################################
  # Report on Compiler (linker) usage by Count
  linkA = CompilerUsageByCount(cursor)
  linkA.build(args, start_date, end_date)
  resultA = linkA.report_by(args, "count")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rl")
  print("\nCompiler usage by Count\n")
  print(bt.build_tbl())
  
  ############################################################
  # Report on Compiler (linker) usage by Core Hours
  linkA = CompilerUsageByCoreHours(cursor)
  linkA.build(args, start_date, end_date)
  resultA = linkA.report_by(args, "corehours")
  bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrl")
  print("\nCompiler usage by Corehours\n")
  print(bt.build_tbl())
  
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
  
  ############################################################
  #  Report of Library usage by Core Hours for largemem.
  libA = Libraries(cursor)
  libA.build(args, start_date, end_date, "largemem")
  resultA = libA.report_by(args,"corehours")
  if (resultA):
    bt      = BeautifulTbl(tbl=resultA, gap = 2, justify = "rrrrl")
    print("")
    print("-------------------------------------------------------------------")
    print("Libraries used by MPI Executables sorted by Core Hours for largemem")
    print("-------------------------------------------------------------------")
    print("")
    print(bt.build_tbl())

if ( __name__ == '__main__'): main()
