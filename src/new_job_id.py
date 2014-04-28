#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
from XALTdb     import XALTdb
import os, sys, MySQLdb 
import warnings
warnings.filterwarnings("ignore", "Unknown table.*")


def main():
  ConfigBaseNm = "xalt_db"
  ConfigFn     = ConfigBaseNm + ".conf"
  dirNm,prg = os.path.split(sys.argv[0])

  if (not os.path.isdir(ConfigFn)):
    ConfigFn = os.path.join(dirNm, ConfigFn)


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
