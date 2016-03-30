import os, sys, re, base64
import MySQLdb, ConfigParser, argparse
import time
from datetime import datetime, timedelta
from Rmap_XALT import Rmap
from xalt_util import *

XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR")
ConfigFn = os.path.join(XALT_ETC_DIR,"xalt_db.conf")

parser = argparse.ArgumentParser \
          (description='Get the last time software provided by \
                        modulefile was used on SYSHOST')
parser.add_argument \
          ("syshost", metavar="SYSHOST", action="store", \
           help="The syshost for this query.")
parser.add_argument('--link_only', action='store_true', \
           help="Get last linked information only")
parser.add_argument('--run_only', action='store_true', \
           help="Get last run information only")
           
args = parser.parse_args()


config = ConfigParser.ConfigParser()     
config.read(ConfigFn)

conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
cursor = conn.cursor()


query = "SELECT DISTINCT(module_name) FROM xalt_object \
          WHERE syshost = '%s' \
            AND module_name is not NULL ORDER by module_name" % args.syshost
cursor.execute(query)
results = cursor.fetchall()

lib_modules = []
for module_name in results:
  lib_modules.append(module_name[0])


query = "SELECT DISTINCT(module_name) FROM xalt_run \
            WHERE module_name is not NULL ORDER by module_name"
cursor.execute(query)
results = cursor.fetchall()

bin_modules = []
for module_name in results:
  bin_modules.append(module_name[0])

#-- Also get module name from ReverseMap file if exists

XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR")
RMapFn   = os.path.join(XALT_ETC_DIR,"reverseMapD")
reverseMapT = Rmap(RMapFn).reverseMapT()

for object in reverseMapT:
  if reverseMapT[object]["kind"] == "lib":
    lib_modules.append(reverseMapT[object]["pkg"])
  if reverseMapT[object]["kind"] == "bin":
    bin_modules.append(str(reverseMapT[object]["pkg"]))


if not args.run_only: 
  #-- make uniq and sort
  moduleset = set(lib_modules)
  modules = list(moduleset)
  modules.sort()
  
  print ''
  print '======================================================================'
  print '%35s %22s %10s' % ('Module Name', 'Last Linked', 'By User')
  print '======================================================================'

  for module_name in modules:
      
    query = "SELECT date, build_user FROM xalt_link \
              WHERE date = \
               ( SELECT max(date) \
                   FROM xalt_link xl, join_link_object jl, xalt_object xo \
                  WHERE xl.link_id = jl.link_id \
                    AND jl.obj_id = xo.obj_id \
                    AND build_syshost = '%s' \
                    AND module_name = '%s')" \
            % (args.syshost, module_name)
    cursor.execute(query)
    usage = cursor.fetchall()
    if not usage:
      max_date, build_user = 'N/A', 'N/A'
    else:
      (max_date, build_user) = usage[0]
    print '%35s %22s %10s' % (module_name[:35], max_date, build_user)
  

if not args.link_only:

  #-- make uniq and sort
  moduleset = set(bin_modules)
  modules = list(moduleset)
  modules.sort()
  
  print ''
  print '======================================================================'
  print '%25s %22s %10s %8s' % ('Module Name', 'Last Run', 'By User', 'Job ID')
  print '======================================================================'

  
  for module_name in modules:
    if module_name == 'NULL':
      continue
    
    query = "SELECT FROM_UNIXTIME(start_time), user, job_id \
               FROM xalt_run \
              WHERE module_name = '%s' \
                AND syshost = '%s' ORDER BY start_time DESC limit 1" \
              % (module_name, args.syshost)
    cursor.execute(query)
    runs = cursor.fetchall()
    if not runs:
      max_date, run_user, job_id = 'N/A', 'N/A', 'N/A'
    else:
      (max_date, run_user, job_id) = runs[0]
    
    print '%25s %22s %10s %8s' \
            % (module_name[:25], max_date, run_user, job_id[:6])
