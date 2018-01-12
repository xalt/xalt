#------------------------------------------------------------------------    #
# This python script gets the counts of Link or Run over a period in bins.   #
#                                                                            #
# Examples:                                                                  #
#                                                                            #
# 1. Show the help output:                                                   #
#   python LinkRunCountByPeriod.py -h                                        #
#                                                                            #
# 2. Get monthly Link and Run count for all systems in 2017                  #
#   python ./LinkRunCountByPeriod.py --bin month \                           #
#   --startdate 2017-01-01                                                   #
#                                                                            #
#                                                                            #
#----------------------------------------------------------------------------#

import os, sys, re, base64, operator
import MySQLdb, argparse
import time
import calendar
from datetime import datetime, timedelta, date
try:
  import configparser
except:
  import ConfigParser as configparser
  
  
def add_months(sourcedate, months):
  month = sourcedate.month - 1 + months
  year = sourcedate.year + month // 12
  month = month % 12 + 1
  day = min(sourcedate.day,calendar.monthrange(year,month)[1])
  return datetime(year, month, day)

XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR")
ConfigFn = os.path.join(XALT_ETC_DIR,"xalt_db.conf")

parser = argparse.ArgumentParser \
          (description='Get count of link or run events.')

parser.add_argument \
          ("--syshost", dest="syshost", action="store", \
           help="limit to this SYSHOST only.")
parser.add_argument \
          ("--startdate", dest='startdate', action="store", \
           help="exclude everything before STARTDATE (in YYYY-MM-DD). \
                 If not specified, defaults to ENDDATE - 90 days.")
parser.add_argument \
          ("--enddate", dest='enddate', action="store", \
           help="exclude everything after ENDDATE (in YYYY-MM-DD). \
                 If not specified, defaults to today.")
parser.add_argument \
          ("--bin", dest="bin", action="store", \
           help="binning of the selected period. \
                 Valid values are 'day' (default), 'week', 'month'.")
              
args = parser.parse_args()

enddate = time.strftime('%Y-%m-%d')
if args.enddate is not None:
  enddate = args.enddate

startdate = (datetime.strptime(enddate, "%Y-%m-%d") - timedelta(90)) \
             .strftime('%Y-%m-%d');
if args.startdate is not None:
  startdate = args.startdate

#-- make the range inclusive in mysql
#enddate   += " 23:59:59"
#startdate += " 00:00:00"
  
bin = 'day'
if args.bin is not None:
  if ( args.bin == 'day' 
       or args.bin == 'week' 
       or args.bin == 'month' ):
    bin = args.bin
  
config = configparser.ConfigParser()     
config.read(ConfigFn)

conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
cursor = conn.cursor()

print ""
print "===================================================================="
print " %sly Link & Run Counts for Period %s - %s" % (bin.title(), startdate, enddate)
print "===================================================================="


s_dt   = datetime.strptime(startdate, "%Y-%m-%d")
s_date = s_dt.strftime('%Y-%m-%d 00:00:01')
iB     = 1
while (s_dt < datetime.strptime(enddate, "%Y-%m-%d")):
  if bin == 'day':
    e_dt   = s_dt + timedelta(days=1)
  if bin == 'week':
    e_dt   = s_dt + timedelta(weeks=1)
  if bin == 'month':
    d_t  = s_date[:10].split('-')
    e_dt = add_months(date(int(d_t[0]), int(d_t[1]), int(d_t[2])), 1) 
  
  s_date = s_dt.strftime('%Y-%m-%d 00:00:01');
  e_date = e_dt.strftime('%Y-%m-%d 00:00:00');
  
  query = "SELECT count(*) as count \
             FROM xalt_link \
            WHERE date >= '%s' AND date <= '%s'" % (s_date, e_date)
  if args.syshost is not None:
    query += " AND build_syshost = '%s'" % args.syshost
  cursor.execute(query)
  link = cursor.fetchone()
  
  query = "SELECT count(*) as count \
             FROM xalt_run \
            WHERE date >= '%s' AND date <= '%s'" % (s_date, e_date)
  if args.syshost is not None:
    query += " AND syshost = '%s'" % args.syshost
  cursor.execute(query)
  run = cursor.fetchone()
  
  print "%10s %3d %10s %10s" % (bin, iB, link[0], run[0])
    
  iB += 1
  s_dt, s_date = e_dt, e_date



