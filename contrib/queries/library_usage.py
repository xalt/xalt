#------------------------------------------------------------------------    #
# This python script gets library usage on SYSHOST grouped by module name.   #
# The number of linking instances and unique user for each library is        #
# displayed. A second table where library version (module name versions) has #
# been agregated is also displayed (assuming module # name version is in the #
# form <module_name/version>).                                               #
#                                                                            #
# Examples:                                                                  #
#                                                                            #
# 1. Show the help output:                                                   #
#   python library_usage.py -h                                               #
#                                                                            #
# 2. Get library usage on Darter for the last 90 days                        #
#   python library_usage.py darter                                           #
#                                                                            #
# 3. Get library usage on Darter for specific period                         #
#   python library_usage.py darter --startdate 2015-03-01 \                  #
#          --endate 2015-06-31                                               #
#                                                                            #
# 4. Get library usage on Darter for the last 90 days, excluding all         #
#    module name with 'ari', 'gcc', and 'craype' in its name                 #
#   python library_usage.py darter --exclude ari,gcc,craype                  #
#                                                                            #
#----------------------------------------------------------------------------#

import os, sys, re, base64, operator
import MySQLdb, argparse
import time
from datetime import datetime, timedelta
try:
  import configparser
except:
  import ConfigParser as configparser

XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR")
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
  
config = configparser.ConfigParser()     
config.read(ConfigFn)

conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
cursor = conn.cursor()

#-- Build query
query = "SELECT u.module_name, COUNT(u.date) AS count \
           FROM (SELECT module_name, object_path, join_link_object.date \
                   FROM xalt_link, join_link_object, xalt_object \
                  WHERE build_syshost = '%s' \
                    AND xalt_link.link_id = join_link_object.link_id \
                    AND join_link_object.obj_id = xalt_object.obj_id \
                    AND join_link_object.date >= '%s' AND join_link_object.date <= '%s' \
        " % (args.syshost, startdate, enddate)
if excludePatterns is not None:
  for x in excludePatterns:
    query = query + "AND module_name NOT LIKE '%%%s%%'" % x
query = query + "AND module_name IS NOT NULL \
                  GROUP BY module_name, date) AS u \
          GROUP BY u.module_name \
          ORDER BY count DESC" 

#-- Alternative, simpler queries 
#query = "SELECT module_name, COUNT(DISTINCT(date)) as count \
#                   FROM xalt_link, join_link_object, xalt_object \
#                  WHERE build_syshost = '%s' \
#                    AND xalt_link.link_id = join_link_object.link_id \
#                    AND join_link_object.obj_id = xalt_object.obj_id \
#                    AND date >= '%s' AND date <= '%s' \
#                    AND module_name NOT LIKE 'gcc/%%' \
#                    AND module_name NOT LIKE 'cce/%%' \
#                    AND module_name NOT LIKE 'intel/%%' \
#                    AND module_name NOT LIKE '%%.ari' \
#                    AND module_name NOT LIKE 'cray-mpich%%' \
#                    AND module_name NOT LIKE 'craype-%%' \
#                    AND module_name IS NOT NULL \
#                  GROUP BY module_name, date \
#          ORDER BY count DESC" % (args.syshost, startdate, enddate)
#

cursor.execute(query)
results = cursor.fetchall()

print ""
print "===================================================================="
print "%35s %10s %10s" % ("Libraries (with ver)", "# Inst.", "# Unq.Usr")
print "===================================================================="

mydict = {}
for module_name, count in results:
  pos = module_name.find("/")
  if pos == -1:
    pos = len(module_name)
  mydict[module_name[:pos]] = 0
  query = "SELECT count(distinct(build_user)) as distinct_user \
             FROM xalt_link, join_link_object, xalt_object \
            WHERE build_syshost = '%s' \
              AND xalt_link.link_id = join_link_object.link_id \
              AND join_link_object.obj_id = xalt_object.obj_id \
              AND join_link_object.date >= '%s' AND join_link_object.date <= '%s' \
              AND module_name = '%s'" \
                % ( args.syshost, startdate, enddate, module_name )
  cursor.execute(query)
  d_user = cursor.fetchall()
  print "%35s %10s %10s" % (module_name, count, d_user[0][0])
  
  
#-- Aggregate versions
for module_name, count in results:
  pos = module_name.find("/")
  if pos == -1:
    pos = len(module_name)
  mydict[module_name[:pos]] = mydict[module_name[:pos]] + int(count)

sorted_x = sorted(mydict.items(), key=operator.itemgetter(1), reverse=True)

print ""
print "===================================================================="
print "%35s %10s %10s" % ("Libraries", "# Inst.", "# Unq.Usr")
print "===================================================================="

for module_name, count in sorted_x:
  query = "SELECT count(distinct(build_user)) as distinct_user \
             FROM xalt_link, join_link_object, xalt_object \
            WHERE build_syshost = '%s' \
              AND xalt_link.link_id = join_link_object.link_id \
              AND join_link_object.obj_id = xalt_object.obj_id \
              AND join_link_object.date >= '%s' AND join_link_object.date <= '%s' \
              AND module_name LIKE '%s/%%'" \
                % ( args.syshost, startdate, enddate, module_name )
  cursor.execute(query)
  d_user = cursor.fetchall()
  print "%35s %10s %10s" % (module_name, count, d_user[0][0])

#print sorted_x
