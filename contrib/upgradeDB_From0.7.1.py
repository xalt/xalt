#!/usr/bin/env python
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
import os, sys, re, MySQLdb

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../libexec")))

from XALTdb     import XALTdb
from xalt_util  import dbConfigFn
import argparse
class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--dbname",      dest='dbname', action="store",      default = "xalt", help="xalt")
    args = parser.parse_args()
    return args

def main():
  """
  This program upgrade the Database used by XALT to the current version 
  from the version specified in script name.
  """

  args     = CmdLineOptions().execute()
  configFn = dbConfigFn(args.dbname)

  if (not os.path.isfile(configFn)):
    dirNm, exe = os.path.split(sys.argv[0])
    fn         = os.path.join(dirNm, configFn)
    if (os.path.isfile(fn)):
      configFn = fn
    else:
      configFn = os.path.abspath(os.path.join(dirNm, "../site", configFn))
      
  xalt = XALTdb(configFn)
  db   = xalt.db()

  try:
    conn   = xalt.connect()
    cursor = conn.cursor()

    # If MySQL version < 4.1, comment out the line below
    cursor.execute("SET SQL_MODE=\"NO_AUTO_VALUE_ON_ZERO\"")
    cursor.execute("USE "+xalt.db())

    idx = 1

    print("start")
    
    ###########################################################################
    # NOTE: New DB schema createDB.py uses unsigned int for some columns, but #
    #       modifying a signed to unsigned is problematic for columns already #
    #       referenced as FOREIGN KEYS. Therefor this script does not update  #
    #       those columns.                                                    #
    ###########################################################################

    # 1
    cursor.execute("""
        ALTER TABLE `xalt_link`
          MODIFY COLUMN `link_program`     varchar(64)        NOT NULL,
             ADD COLUMN `link_path`        varchar(1024)      NOT NULL AFTER `link_program`,
             ADD COLUMN `link_module_name` varchar(64)        AFTER `link_path`,
             ADD COLUMN `link_line`        blob               AFTER `link_module_name`,
             ADD INDEX  `index_date` (`date`)
          """)
    print("(%d) upgraded xalt_link table" % idx); idx += 1
    
    # 4
    cursor.execute("""
        ALTER TABLE `xalt_run`
          MODIFY COLUMN `job_id`        char(64)             NOT NULL,
          MODIFY COLUMN `num_cores`     int(11)     unsigned NOT NULL,
          MODIFY COLUMN `job_num_cores` int(11)     unsigned NOT NULL,
          MODIFY COLUMN `num_nodes`     int(11)     unsigned NOT NULL,
          MODIFY COLUMN `num_threads`   smallint(6) unsigned NOT NULL,
          MODIFY COLUMN `exit_code`     smallint(6)          NOT NULL,
             ADD COLUMN `cmdline`       blob                 NOT NULL AFTER `cwd`,
             ADD  INDEX `index_date`     (`date`     )
          """)
    print("(%d) upgraded xalt_run table" % idx); idx += 1

    # 6
    cursor.execute("""
        ALTER TABLE `xalt_env_name`
             ADD  INDEX `a_env_name` (`env_name`)
        """)
    print("(%d) upgraded xalt_env_name table" % idx)
    idx += 1

    # 7
    cursor.execute("""
        ALTER TABLE `join_run_env`
          MODIFY COLUMN `join_id` bigint(20) unsigned   NOT NULL auto_increment
        """)
    print("(%d) upgraded join_run_env table" % idx)
    idx += 1
    
    #11
    cursor.execute("""
        ALTER TABLE `join_link_function`
          MODIFY COLUMN `join_id` int(11) unsigned NOT NULL auto_increment
        """)
    print("(%d) upgraded xalt_function table" % idx)
    idx += 1
    
    # 12
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_total_env` (
          `envT_id`       bigint(20) unsigned NOT NULL auto_increment,
          `run_id`        int(11)             NOT NULL,
          `env_blob`      blob                NOT NULL,
          PRIMARY KEY (`envT_id`),
          FOREIGN KEY (`run_id`)  REFERENCES `xalt_run`(`run_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)
    print("(%d) created xalt_env table" % idx); idx += 1

    cursor.close()
  except  MySQLdb.Error as e:
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)

if ( __name__ == '__main__'): main()
