#------------------------------------------------------------------------    #
# This python script gets executable usage on SYSHOST grouped by exec name.  #
# Total CPU time used, number of jobs, and number of users of the exec are   #
# shown. Executable with "known" names are shown as all CAPS and grouped     #
# together even if they have different actual exec name / version, other     #
# executables are only grouped by their name.                                #            #
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
import os, sys, re, base64, operator
import MySQLdb, ConfigParser, argparse
import time
from datetime import datetime, timedelta

XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR","./")
ConfigFn = os.path.join(XALT_ETC_DIR,"xalt_db.conf")

parser = argparse.ArgumentParser \
          (description='Get library usage on SYSHOST grouped by module name.\
           The number of linking instances and unique user for each library \
           is displayed. A second table where library version (module name \
           versions) has been agregated is also displayed (assuming module \
           name version is in the form <module_name/version>).')
parser.add_argument \
          ("syshost", metavar="SYSHOST", action="store", \
           help="The syshost for this query.")
parser.add_argument \
          ("--startdate", dest='startdate', action="store", \
           help="exclude everything before STARTDATE (in YYYY-MM-DD). \
                 If not specified, defaults to ENDDATE - 90 days.")
parser.add_argument \
          ("--enddate", dest='enddate', action="store", \
           help="exclude everything after ENDDATE (in YYYY-MM-DD). \
                 If not specified, defaults to today.")
parser.add_argument \
          ("--exclude", dest='patterns', action="store", \
           help="comma separated PATTERN to ignore module name whose \
                 substring matches any of the PATTERNS.")

args = parser.parse_args()

enddate = time.strftime('%Y-%m-%d')
if args.enddate is not None:
  enddate = args.enddate

startdate = (datetime.strptime(enddate, "%Y-%m-%d") - timedelta(90)) \
             .strftime('%Y-%m-%d');
if args.startdate is not None:
  startdate = args.startdate
  
excludePatterns = None
if args.patterns is not None:
  excludePatterns = [x.strip() for x in args.patterns.split(',')]
  
config = ConfigParser.ConfigParser()     
config.read(ConfigFn)

conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
cursor = conn.cursor()

query = "SELECT CASE \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'wrf' then 'WRF*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'chim' then 'CHIMERA*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'vasp' then 'VASP*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'namd' then 'NAMD*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'lmp' then 'LAMMPS*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'amber' then 'AMBER*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'charm' then 'CHARMM*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'enzo' then 'ENZO*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'gromacs' then 'GROMACS*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'cp2k' then 'CP2K*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'nwchem' then 'NWCHEM*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'ttmmd' then 'TTMMD*' \
            WHEN LOWER(xalt_run.exec_path)  REGEXP 'genasis' then 'GENASIS*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'engine_par' then 'VISIT*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'foam' then 'OPENFOAM*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'ph.x' then 'Q-ESPRESSO*' \
            WHEN LOWER(SUBSTRING_INDEX(xalt_run.exec_path,'/',-1)) REGEXP 'pw.x' then 'Q-ESPRESSO*' \
            ELSE SUBSTRING_INDEX(xalt_run.exec_path,'/',-1) END \
          AS execname, ROUND(SUM(run_time*num_cores/3600)) as totalcput, \
          COUNT(date) as n_jobs, COUNT(DISTINCT(user)) as n_users \
          FROM xalt_run \
         WHERE syshost = '%s' \
           AND date >= '%s 00:00:00' AND date <= '%s 23:59:59' \
         GROUP BY execname ORDER BY totalcput DESC" \
         % (args.syshost, startdate, enddate)
cursor.execute(query)
results = cursor.fetchall()

print ("")
print ("====================================================================")
print ("%35s %10s %10s %10s" % ("Exec", "CPU Time.", "# Jobs", "# Users"))
print ("====================================================================")

sum = 0.0
for execname, totalcput, n_jobs, n_users in results:
  sum += totalcput
  print ("%35s %10s %10s %10s" % (execname, totalcput, n_jobs, n_users))

print("(M) SUs", sum/1.0e6, file=sys.stderr)

