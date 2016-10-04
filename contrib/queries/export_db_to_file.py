#----------------------------------------------------------------------------#
#                                                                            #
# This python script exports DB entries for a SYSHOST to files, which can be #
# dumped back to DB using xalt_file_to_db script. This is useful for DB      #
# migration of a particular SYSHOST, since this script gets all of the       #
# one-to-many DB entries.                                                    #
#                                                                            #
#----------------------------------------------------------------------------#

from __future__ import print_function
import os, sys, re, base64, operator, json
import MySQLdb, argparse
import time
from datetime import datetime, timedelta

try:
  import configparser
except:
  import ConfigParser as configparser
  
XALT_DIR     = os.environ.get("XALT_DIR")
XALT_ETC_DIR = os.environ.get("XALT_ETC_DIR")
ConfigFn = os.path.join(XALT_ETC_DIR,"xalt_db.conf")

sys.path.insert(1,os.path.realpath(os.path.join(XALT_DIR, "libexec")))
from xalt_transmission_factory import XALT_transmission_factory

parser = argparse.ArgumentParser \
          (description='Export DB entries for a SYSHOST to files.')
parser.add_argument \
          ("syshost", metavar="SYSHOST", action="store", \
           help="The syshost for this query.")
parser.add_argument \
          ("--outputdir", dest="outputdir", action="store", \
           help="directory where exported files to be written, \
                 defaults to current working directory.")
parser.add_argument \
          ("--startdate", dest='startdate', action="store", \
           help="exclude everything before STARTDATE (in YYYY-MM-DD). \
                 If not specified, defaults to the beginning of time.")
parser.add_argument \
          ("--enddate", dest='enddate', action="store", \
           help="exclude everything after ENDDATE (in YYYY-MM-DD). \
                 If not specified, defaults to today.")

args = parser.parse_args()

outputdir = os.getcwd()
if args.outputdir is not None:
  outputdir = args.outputdir

enddate = time.strftime('%Y-%m-%d')
if args.enddate is not None:
  enddate = args.enddate

startdate = '1970-01-01'
if args.startdate is not None:
  startdate = args.startdate

  
config = configparser.ConfigParser()     
config.read(ConfigFn)

conn = MySQLdb.connect \
         (config.get("MYSQL","HOST"), \
          config.get("MYSQL","USER"), \
          base64.b64decode(config.get("MYSQL","PASSWD")), \
          config.get("MYSQL","DB"))
cursor1 = conn.cursor(MySQLdb.cursors.DictCursor)
cursor2 = conn.cursor(MySQLdb.cursors.DictCursor)

#-- link table
query = "SELECT *, UNCOMPRESS(link_line) as u_link_line \
           FROM xalt_link \
          WHERE build_syshost = '%s' \
            AND date >= '%s' AND date <= '%s'" \
         % (args.syshost, startdate, enddate)
cursor1.execute(query)
nRows = cursor1.rowcount
for i in xrange(0, nRows):
  link_row = cursor1.fetchone()
  
  query = "SELECT object_path, hash_id  \
             FROM xalt_object xo, join_link_object lo \
            WHERE lo.obj_id = xo.obj_id \
              AND lo.link_id = %s" % link_row["link_id"]
  cursor2.execute(query)
  linkA = [ [ iRow['object_path'], iRow['hash_id'] ] \
            for iRow in cursor2.fetchall() ]
  
  query = "SELECT function_name  \
             FROM xalt_function xf, join_link_function lf \
            WHERE lf.func_id = xf.func_id \
              AND lf.link_id = %s" % link_row["link_id"]
  cursor2.execute(query)
  function = [ iRow['function_name'] for iRow in cursor2.fetchall() ]
  
  resultT                  = {}
  resultT['uuid']          = link_row['uuid']
  resultT['link_program']  = link_row['link_program']
  resultT['link_path']     = link_row['link_path']
  resultT['link_line']     = link_row['u_link_line']
  resultT['build_user']    = link_row['build_user']
  resultT['exit_code']     = link_row['exit_code']
  resultT['build_epoch']   = link_row['build_epoch']
  resultT['exec_path']     = link_row['exec_path']
  resultT['hash_id']       = link_row['hash_id']
  resultT['wd']            = ''
  resultT['build_syshost'] = link_row['build_syshost']
  resultT['function']      = function
  resultT['linkA']         = linkA
  key                      = "link.%s.%s.json" % \
                             (link_row['build_syshost'], link_row['uuid'])
  
  resultFn = os.path.join(outputdir,key)
  print("Writing: " + resultFn)
  xfer = XALT_transmission_factory.build \
         ("file", link_row['build_syshost'], "link", resultFn)
  xfer.save(resultT, key)
  