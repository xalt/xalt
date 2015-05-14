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
  This program creates the Database used by XALT.
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
    # If the database does not exist, create it, otherwise, switch to the database.
    cursor.execute("CREATE DATABASE IF NOT EXISTS %s DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci" % xalt.db())
    cursor.execute("USE "+xalt.db())

    idx = 1


    print("start")

    # 1
    cursor.execute("""
        CREATE TABLE `xalt_link` (
          `link_id`       int(11)        NOT NULL auto_increment,
          `uuid`          char(36)       NOT NULL,
          `hash_id`       char(40)       NOT NULL,
          `date`          DATETIME       NOT NULL,
          `link_program`  varchar(10)    NOT NULL,
          `build_user`    varchar(64)    NOT NULL,
          `build_syshost` varchar(64)    NOT NULL,
          `build_epoch`   double         NOT NULL,
          `exit_code`     tinyint(4)     NOT NULL,
          `exec_path`     varchar(1024)  NOT NULL,
          PRIMARY KEY  (`link_id`),
          UNIQUE  KEY  `uuid` (`uuid`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_link table" % idx); idx += 1

    # 2
    cursor.execute("""
        CREATE TABLE `xalt_object` (
          `obj_id`        int(11)         NOT NULL auto_increment,
          `object_path`   varchar(1024)   NOT NULL,
          `syshost`       varchar(64)     NOT NULL,
          `hash_id`       char(40)        NOT NULL,
          `module_name`   varchar(64)             ,  
          `timestamp`     TIMESTAMP               ,
          `lib_type`      char(2)         NOT NULL,
          PRIMARY KEY  (`obj_id`),
          INDEX  `index_hash_id` (`hash_id`),
          INDEX  `thekey` (`object_path`(128), `hash_id`, `syshost`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_object table" % idx ); idx += 1;


    # 3
    cursor.execute("""
        CREATE TABLE `join_link_object` (
          `join_id`       int(11)        NOT NULL auto_increment,
          `obj_id`        int(11)        NOT NULL,
          `link_id`       int(11)        NOT NULL,
          PRIMARY KEY (`join_id`),
          FOREIGN KEY (`link_id`) REFERENCES `xalt_link`(`link_id`),
          FOREIGN KEY (`obj_id`)  REFERENCES `xalt_object`(`obj_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)
    print("(%d) create join_link_object table" % idx); idx += 1

    # 4
    cursor.execute("""
        CREATE TABLE `xalt_run` (
          `run_id`        int(11)        NOT NULL auto_increment,
          `job_id`        char(11)       NOT NULL,
          `run_uuid`      char(36)       NOT NULL,
          `date`          datetime       NOT NULL,

          `syshost`       varchar(64)    NOT NULL,
          `uuid`          char(36)               ,
          `hash_id`       char(40)       NOT NULL,

          `account`       char(11)       NOT NULL,
          `exec_type`     char(7)        NOT NULL,
          `start_time`    double         NOT NULL,

          `end_time`      double         NOT NULL,
          `run_time`      double         NOT NULL,
          `num_cores`     int(11)        NOT NULL,
          `job_num_cores` int(11)        NOT NULL,

          `num_nodes`     int(11)        NOT NULL,
          `num_threads`   tinyint(4)     NOT NULL,
          `queue`         varchar(32)    NOT NULL,
          `exit_code`     tinyint(4)     NOT NULL,

          `user`          varchar(32)    NOT NULL,
          `exec_path`     varchar(1024)  NOT NULL,
          `module_name`   varchar(64)            ,
          `cwd`           varchar(1024)  NOT NULL,
          PRIMARY KEY            (`run_id`),
          INDEX  `index_run_uuid` (`run_uuid`),
          INDEX `thekey` (`job_id`, `syshost`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_run table" % idx)
    idx += 1

    # 5
    cursor.execute("""
        CREATE TABLE `join_run_object` (
          `join_id`       int(11)        NOT NULL auto_increment,
          `obj_id`        int(11)        NOT NULL,
          `run_id`        int(11)        NOT NULL,

          PRIMARY KEY (`join_id`),
          FOREIGN KEY (`run_id`)  REFERENCES `xalt_run`(`run_id`),
          FOREIGN KEY (`obj_id`)  REFERENCES `xalt_object`(`obj_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)
    print("(%d) create join_run_object table" % idx); idx += 1


    # 6
    cursor.execute("""
        CREATE TABLE `xalt_env_name` (
          `env_id`        int(11)       NOT NULL auto_increment,
          `env_name`      varchar(64)   NOT NULL,
          PRIMARY KEY  (`env_id`),
          UNIQUE  KEY  `env_name` (`env_name`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_env_name table" % idx); idx += 1

    # 7
    cursor.execute("""
        CREATE TABLE `join_run_env` (
          `join_id`       int(11)        NOT NULL auto_increment,
          `env_id`        int(11)        NOT NULL,
          `run_id`        int(11)        NOT NULL,
          `env_value`     blob           NOT NULL,
          PRIMARY KEY (`join_id`),
          FOREIGN KEY (`env_id`)  REFERENCES `xalt_env_name`(`env_id`),
          FOREIGN KEY (`run_id`)  REFERENCES `xalt_run`(`run_id`) 
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1 
        """)
    print("(%d) create join_run_env table" % idx); idx += 1

    # 8 
    cursor.execute("""
        CREATE TABLE `xalt_user` (
          `usr_id`        int(11)        NOT NULL auto_increment,
          `user`          varchar(32)    NOT NULL,
          `anon_user`     varchar(12)    NOT NULL,
          PRIMARY KEY (`usr_id`),
          INDEX `the_user` (`user`),
          INDEX `a_user`   (`anon_user`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_user table" % idx); idx += 1

    # 9 
    cursor.execute("""
        CREATE TABLE `xalt_account` (
          `acct_id`          int(11)        NOT NULL auto_increment,
          `account`          varchar(32)    NOT NULL,
          `anon_user`        varchar(10)    NOT NULL,
          `field_of_science` varchar(64)    NOT NULL,
          PRIMARY KEY (`acct_id`),
          INDEX `the_account` (`account`),
          INDEX `a_acct`   (`anon_acct`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_general_ci AUTO_INCREMENT=1
        """)

    cursor.close()
  except  MySQLdb.Error, e:
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)

if ( __name__ == '__main__'): main()
