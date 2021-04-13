#!/bin/sh
# -*- python -*-

################################################################################
# This file is python 2/3 bilingual. 
# The line """:" starts a comment in python and is a no-op in shell.
""":"
# Shell code to find and run a suitable python interpreter.
for cmd in python3 python python2; do
   command -v > /dev/null $cmd && exec $cmd $0 "$@"
done

echo "Error: Could not find a valid python interpreter --> exiting!" >&2
exit 2
":""" # this line ends the python comment and is a no-op in shell.
################################################################################

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
import os, sys, re, MySQLdb, traceback

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../libexec")))

from XALTdb     import XALTdb
from xalt_util  import dbConfigFn
import argparse

Have_version = True

try:
  from packaging import version
except:
  Have_version = False



def parseVersion(s):
  factorA = [ 1000000, 1000, 1]
  patt    = re.compile(r"^([0-9.]+)")
  m       = patt.match(s)
  if (not m):
    s     = "0.0.0"
  else:
    s     = m.group()

  if (Have_version):
    return version.parse(s)

  a       = s.split(".")
  i       = 0
  v       = 0
  sz      = 3
  for vv in a:
    v     = v + int(vv)*factorA[i]
    i     = i + 1
    if (i >= sz):
      break
    
  return v


class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--drop",        dest='drop',   action="store_true", default = False,          help="xalt")
    parser.add_argument("--dbname",      dest='dbname', action="store",      default = "xalt",         help="xalt")
    parser.add_argument("--confFn",      dest='confFn', action="store",      default = None,           help="xalt")
    args = parser.parse_args()
    return args


def main():
  """
  This program creates the Database used by XALT.
  """

  args     = CmdLineOptions().execute()
  configFn = dbConfigFn(args.dbname)
  if (args.confFn):
    configFn = args.confFn

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

    cursor.execute("SHOW VARIABLES like 'version'")
    row      = cursor.fetchone()
    mysqlV   = row[1]
    currentV = parseVersion(mysqlV)

    if   (currentV >= parseVersion("6.0")):
       cursor.execute("SET SQL_MODE=\"NO_AUTO_VALUE_ON_ZERO\"")
    elif (currentV >  parseVersion("4.1")):
       cursor.execute("SET SQL_MODE=\"NO_AUTO_VALUE_ON_ZERO,NO_AUTO_CREATE_USER\"")

    # drop db if requested.
    if (args.drop):
      cursor.execute("DROP DATABASE IF EXISTS %s " % xalt.db())

    # If the database does not exist, create it, otherwise, switch to the database.
    cursor.execute("CREATE DATABASE IF NOT EXISTS %s DEFAULT CHARACTER SET utf8 COLLATE utf8_bin" % xalt.db())
    cursor.execute("USE "+xalt.db())

    idx = 1


    print("start")

    # 1
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_link` (
          `link_id`          int(11)   unsigned NOT NULL auto_increment,

          `uuid`             char(36)           NOT NULL,
          `hash_id`          char(40)           NOT NULL,
          `date`             DATETIME           NOT NULL,

          `link_program`     varchar(64)        NOT NULL,
          `link_path`        varchar(1024)      NOT NULL,
          `link_module_name` varchar(64)                ,

          `link_line`        blob                       ,
          `cwd`              varchar(1024)              ,
          `build_user`       varchar(64)        NOT NULL,

          `build_syshost`    varchar(64)        NOT NULL,
          `build_epoch`      double             NOT NULL,
          `exec_path`        varchar(1024)      NOT NULL,

          PRIMARY KEY  (`link_id`),
          INDEX  `index_date` (`date`),
          UNIQUE  KEY  `uuid` (`uuid`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_link table" % idx); idx += 1

    # 2
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_object` (
          `obj_id`        int(11)   unsigned NOT NULL auto_increment,
          `object_path`   varchar(1024)      NOT NULL,
          `syshost`       varchar(64)        NOT NULL,
          `hash_id`       char(40)           NOT NULL,
          `module_name`   varchar(64)                ,  
          `timestamp`     TIMESTAMP                  ,
          `lib_type`      char(2)            NOT NULL,
          PRIMARY KEY  (`obj_id`),
          INDEX  `index_hash_id` (`hash_id`),
          INDEX  `thekey` (`object_path`(128), `hash_id`, `syshost`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_object table" % idx ); idx += 1;


    # 3
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `join_link_object` (
          `join_id`       int(11) unsigned   NOT NULL auto_increment,
          `obj_id`        int(11) unsigned   NOT NULL,
          `link_id`       int(11) unsigned   NOT NULL,
          `date`          DATE               NOT NULL,
          PRIMARY KEY (`join_id`),
          FOREIGN KEY (`link_id`) REFERENCES `xalt_link`(`link_id`),
          FOREIGN KEY (`obj_id`)  REFERENCES `xalt_object`(`obj_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create join_link_object table" % idx); idx += 1

    # 4
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_run` (
          `run_id`        int(11)     unsigned NOT NULL auto_increment,
          `job_id`        char(64)             NOT NULL,
          `run_uuid`      char(36)             NOT NULL,
          `date`          datetime             NOT NULL,

          `syshost`       varchar(64)          NOT NULL,
          `uuid`          char(36)                     ,
          `hash_id`       char(40)             NOT NULL,

          `account`       varchar(20)          NOT NULL,
          `exec_type`     char(7)              NOT NULL,
          `start_time`    double               NOT NULL,

          `end_time`      double               NOT NULL,
          `run_time`      double               NOT NULL,
          `probability`   double               NOT NULL,
          `num_cores`     int(11)     unsigned NOT NULL,

          `num_nodes`     int(11)     unsigned NOT NULL,
          `num_threads`   smallint(6) unsigned NOT NULL,
          `num_gpus`      int(11)     unsigned NOT NULL,

          `queue`         varchar(32)          NOT NULL,
          `sum_runs`      int(11)     unsigned NOT NULL,
          `sum_time`      double               NOT NULL,

          `user`          varchar(32)          NOT NULL,
          `exec_path`     varchar(1024)        NOT NULL,
          `module_name`   varchar(64)                  ,
          `cwd`           varchar(1024)        NOT NULL,
          `cmdline`       blob                 NOT NULL,
          `container`     varchar(32)                  ,
          PRIMARY KEY             (`run_id`   ),
          INDEX  `index_date`     (`date`     ),
          INDEX  `index_run_uuid` (`run_uuid` ),
          INDEX `thekey` (`job_id`, `syshost` )
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_run table" % idx)
    idx += 1

    # 5
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `join_run_object` (
          `join_id`       int(11)      unsigned  NOT NULL auto_increment,
          `obj_id`        int(11)      unsigned  NOT NULL,
          `run_id`        int(11)      unsigned  NOT NULL,
          `date`          DATE                   NOT NULL,

          PRIMARY KEY (`join_id`),
          FOREIGN KEY (`run_id`)  REFERENCES `xalt_run`(`run_id`),
          FOREIGN KEY (`obj_id`)  REFERENCES `xalt_object`(`obj_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create join_run_object table" % idx); idx += 1


    # 6
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_env_name` (
          `env_id`        int(20) unsigned  NOT NULL auto_increment,
          `env_name`      varchar(64)       NOT NULL,
          PRIMARY KEY  (`env_id`),
          UNIQUE  KEY  `env_name` (`env_name`),
          INDEX        `a_env_name` (`env_name`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_env_name table" % idx); idx += 1

    # 7
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `join_run_env` (
          `join_id`       bigint(20) unsigned   NOT NULL auto_increment,
          `env_id`        int(11)    unsigned   NOT NULL,
          `run_id`        int(11)    unsigned   NOT NULL,
          `date`          DATE                  NOT NULL,
          `env_value`     blob                  NOT NULL,
          PRIMARY KEY (`join_id`),
          FOREIGN KEY (`env_id`)  REFERENCES `xalt_env_name`(`env_id`),
          FOREIGN KEY (`run_id`)  REFERENCES `xalt_run`(`run_id`) 
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1 
        """)
    print("(%d) create join_run_env table" % idx); idx += 1

    # 8 
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_user` (
          `usr_id`        int(11)     unsigned NOT NULL auto_increment,
          `user`          varchar(32)          NOT NULL,
          `anon_user`     varchar(12)          NOT NULL,
          PRIMARY KEY (`usr_id`),
          INDEX `the_user` (`user`),
          INDEX `a_user`   (`anon_user`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_user table" % idx); idx += 1

    # 9 
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_account` (
          `acct_id`          int(11)      unsigned NOT NULL auto_increment,
          `account`          varchar(32)           NOT NULL,
          `anon_user`        varchar(10)           NOT NULL,
          `field_of_science` varchar(64)           NOT NULL,
          PRIMARY KEY (`acct_id`),
          INDEX `the_account` (`account`),
          INDEX `a_acct`   (`anon_user`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_account table" % idx ); idx += 1;
    
    # 10
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_function` (
          `func_id`       int(11)        unsigned NOT NULL auto_increment,
          `function_name` varchar(255)            NOT NULL,
          PRIMARY KEY  (`func_id`),
          UNIQUE  KEY  `function_name` (`function_name`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_function table" % idx ); idx += 1;
    
    # 11
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `join_link_function` (
          `join_id`       int(11)       unsigned NOT NULL auto_increment,
          `func_id`       int(11)       unsigned NOT NULL,
          `link_id`       int(11)       unsigned NOT NULL,
          `date`          DATE                   NOT NULL,
          PRIMARY KEY (`join_id`),
          FOREIGN KEY (`func_id`)  REFERENCES `xalt_function`(`func_id`),
          FOREIGN KEY (`link_id`)  REFERENCES `xalt_link`(`link_id`) 
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1 
        """)
    print("(%d) create join_link_function table" % idx); idx += 1
    
    # 12
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_total_env` (
          `envT_id`       bigint(20) unsigned NOT NULL auto_increment,
          `run_id`        int(11)    unsigned NOT NULL,
          `date`          DATE                NOT NULL,
          `env_blob`      blob                NOT NULL,
          PRIMARY KEY (`envT_id`),
          FOREIGN KEY (`run_id`)  REFERENCES `xalt_run`(`run_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_env table" % idx); idx += 1


    # 13 
    cursor.execute("""
        ALTER TABLE `join_link_function` 
         ADD UNIQUE `unique_func_link_id` ( `func_id`, `link_id` )
        """)
    print("(%d) create unique key on join_link_function table" % idx); idx += 1

    # 14
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_pkg` (
          `pkg_id`        bigint(20)     unsigned NOT NULL auto_increment,
          `run_id`        int(11)        unsigned NOT NULL,
          `program`       varchar(12)             NOT NULL,
          `pkg_name`      varchar(64)             NOT NULL,
          `pkg_version`   varchar(32)                     ,
          `pkg_path`      varchar(1024)                   ,
          PRIMARY KEY (`pkg_id`),
          FOREIGN KEY (`run_id`)  REFERENCES `xalt_run`(`run_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_pkg table" % idx); idx += 1
    
    # 15
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS `xalt_field_of_science` (
          `fos_id`           int(11)        unsigned NOT NULL auto_increment,
          `account`          varchar(20)             NOT NULL,
          `field_of_science` varchar(80)             NOT NULL,
          `directorate`      varchar(80)             NOT NULL,
          PRIMARY KEY (`fos_id`)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8  COLLATE=utf8_bin AUTO_INCREMENT=1
        """)
    print("(%d) create xalt_field_of_science table" % idx); idx += 1
    
    cursor.close()
  except  MySQLdb.Error as e:
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)

if ( __name__ == '__main__'): main()
