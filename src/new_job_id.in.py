# -*- python -*-
#
# Git Version: @git@

#-----------------------------------------------------------------------
# XALT: A tool that tracks users jobs and environments on a cluster.
# Copyright (C) 2013-2014 University of Texas at Austin
# Copyright (C) 2013-2014 University of Tennessee
# 
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of 
# the License, or (at your option) any later version. 
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser  General Public License for more details. 
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA
#-----------------------------------------------------------------------
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
  except MySQLdb.Error, e:
    print("-1")
    sys.exit(1)
    
  

  try:
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
