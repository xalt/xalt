#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

from __future__ import print_function
import os, sys, MySQLdb 
dirNm, execName = os.path.split(sys.argv[0])
sys.path.append(os.path.abspath(os.path.join(dirNm, "../libexec")))

from XALTdb     import XALTdb
import warnings
warnings.filterwarnings("ignore", "Unknown table.*")


def main():
  ConfigBaseNm = "xalt_db"
  ConfigFn     = ConfigBaseNm + ".conf"

  if (not os.path.isfile(ConfigFn)):
    dirNm,prg = os.path.split(sys.argv[0])
    fn        = os.path.join(dirNm, ConfigFn)
    if (os.path.isfile(fn)):
      ConfigFn = fn
    else:
      ConfigFn = os.path.abspath(os.path.join(dirNm,"../sbin",ConfigFn))


  xalt = XALTdb(ConfigFn)
  try:
    conn   = xalt.connect()
    query  = "USE "+xalt.db()
    conn.query(query)
    query  = "SELECT job_id FROM xalt_job_id WHERE inc_id='1'"
    conn.query(query)
    result = conn.store_result()
    if (result.num_rows() > 0):
      row    = result.fetch_row()
      job_id = int(row[0][0]) + 1
      query  = "UPDATE xalt_job_id SET job_id='%d' WHERE inc_id='1'" % job_id
      conn.query(query)

      print(job_id)
      




  except MySQLdb.Error, e:
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)

if ( __name__ == '__main__'): main()
