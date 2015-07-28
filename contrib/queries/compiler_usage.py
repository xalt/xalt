#------------------------------------------------------------------------    #
# This python script gets the number of time a compiler (i.e. link_program)  #
# on SYSHOST was used.                                                       #
#                                                                            #
# Examples:                                                                  #
#                                                                            #
# 1. Show the help output:                                                   #
#   python compiler_usage.py -h                                              #
#                                                                            #
# 2. Get compiler usage on Darter for the last 90 days                       #
#   python compiler_usage.py darter                                          #
#                                                                            #
# 3. Get compiler usage on Darter for specific period                        #
#   python compiler_usage.py darter --startdate 2015-03-01 \                 #
#          --endate 2015-06-31                                               #
#                                                                            #
#----------------------------------------------------------------------------#

import os, sys, re, base64, operator
import MySQLdb, ConfigParser, argparse
import time
from datetime import datetime, timedelta

XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR")
ConfigFn = os.path.join(XALT_ETC_DIR,"xalt_db.conf")

parser = argparse.ArgumentParser \
          (description='Get compiler (link_program) usage on SYSHOST.')

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

args = parser.parse_args()

enddate = time.strftime('%Y-%m-%d')
if args.enddate is not None:
  enddate = args.enddate

startdate = (datetime.strptime(enddate, "%Y-%m-%d") - timedelta(90)) \
             .strftime('%Y-%m-%d');
if args.startdate is not None:
  startdate = args.startdate
  
config = ConfigParser.ConfigParser()     
config.read(ConfigFn)

conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
cursor = conn.cursor()

#-- Build query
query = "SELECT link_program, count(*) as count \
           FROM xalt_link \
          WHERE build_syshost = '%s' \
            AND date >= '%s' AND date <= '%s' \
            AND link_program is NOT NULL \
          GROUP BY link_program" % (args.syshost, startdate, enddate)
       
cursor.execute(query)
results = cursor.fetchall()

print ""
print "===================================================================="
print "%35s %10s " % ("Link Program        ", "# Inst." )
print "===================================================================="

for link_program, count in results:
  print "%35s %10s" % (link_program, count)
