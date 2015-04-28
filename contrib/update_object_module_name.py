# update_object_module_name.py
#
# This script updates the module_name if it's NULL in xalt_object table using 
# output from ReverseMap. This is useful for the two cases:
#  1. User didn't initially install Lmod/Spider (thus no ReverseMap file), and now
#     decide to want that after all
#  2. The ReverseMap file is updated after the fact (e.g. lagging with respect to
#     the availability of new modulefile / software, or corrected for whatever
#     reason), so previous entries to DB didn't get the correct module_name
#
#Examples:
#
#$ python update_object_module_name.py --dry-run --syshost=darter --exclude_path=/nics/a/home 
#     --exclude_path=/lustre/medusa --exclude_path=/nics/b/home --exclude_path=/nics/c/home 
#     --exclude_path=/nics/d/home
#
#Found 0 entries out of 92338 to update
#
#$ python update_object_module_name.py --help
#usage: update_object_module_name.py [-h] [--dry-run] [--syshost SYSHOST]
#                                    [--exclude_path EXCLUDE_PATH]
#
#Update XALT_OBJECT table with module_name from reverseMap file where
#module_name is NULL.
#
#optional arguments:
#  -h, --help            show this help message and exit
#  --dry-run             trial run without updating the database
#  --syshost SYSHOST     limit operations to the SYSHOST
#  --exclude_path EXCLUDE_PATH
#                        exclude objects whose paths partially match
#                        EXCLUDE_PATH. This option can be specified multiple
#

import os, sys, re, base64
import MySQLdb, ConfigParser, argparse
from XALT_Rmap import Rmap
from xalt_util import *

XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR")
ConfigFn = os.path.join(XALT_ETC_DIR,"xalt_db.conf")
RMapFn   = os.path.join(XALT_ETC_DIR,"reverseMapD")

parser = argparse.ArgumentParser \
          (description='Update XALT_OBJECT table with module_name from \
                        reverseMap file where module_name is NULL.')
parser.add_argument \
          ("--dry-run", dest='dryrun', action="store_true", \
           help="trial run without updating the database")
parser.add_argument \
          ("--syshost", dest='syshost', action="store", \
           help="limit operations to the SYSHOST")
parser.add_argument \
          ("--exclude_path", dest='exclude_path', action="append", \
           help="exclude objects whose paths partially match EXCLUDE_PATH. \
                 This option can be specified multiple times.")
args = parser.parse_args()


config = ConfigParser.ConfigParser()     
config.read(ConfigFn)

conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
cursor = conn.cursor()

reverseMapT = Rmap(RMapFn).reverseMapT()

#-- Test query          

query = "SELECT obj_id, object_path \
           FROM xalt_object \
          WHERE module_name is NULL"

if args.syshost is not None:
  query = query + " AND syshost = '%s'" % args.syshost

if args.exclude_path is not None:
  for path in args.exclude_path:
    query = query + " AND object_path not like '%%%s%%'" % path

cursor.execute(query)
results = cursor.fetchall()

n_update = 0
for obj_id, obj_path in results:
  module_name = obj2module (obj_path, reverseMapT)
  if module_name != 'NULL':
    n_update = n_update + 1
    query = "UPDATE xalt_object SET module_name = %s WHERE obj_id = %s " \
             % (module_name, obj_id)
    
    if args.dryrun:
      print 'obj_path: ' + obj_path
      print 'module  : ' + module_name
      print 'SQL     : ' + query
      print ''
    else:
      cursor.execute(query)

if args.dryrun:
  print 'Found %d entries out of %d to update' % (n_update, len(results))
else:
  conn.commit()
  print 'Updated %d entries out of %d objects' % (n_update, len(results))

