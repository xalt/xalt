#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

from __future__ import print_function
import os, sys, re, MySQLdb

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../libexec")))

from XALTdb     import XALTdb




def main():

  ConfigBaseNm = "xalt_db"
  ConfigFn     = ConfigBaseNm + ".conf"

  if (not os.path.isfile(ConfigFn)):
    dirNm, exe = os.path.split(sys.argv[0])
    fn         = os.path.join(dirNm, ConfigFn)
    if (os.path.isfile(fn)):
      ConfigFn = fn
    else:
      ConfigFn = os.path.abspath(os.path.join(dirNm, "../site", ConfigFn))
      
  xalt = XALTdb(ConfigFn)
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
        ) ENGINE=MyISAM DEFAULT CHARSET=latin1  COLLATE=latin1_bin AUTO_INCREMENT=1
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
          UNIQUE KEY `thekey` (`object_path`(512), `hash_id`, `syshost`)
        ) ENGINE=MyISAM DEFAULT CHARSET=latin1  COLLATE=latin1_bin AUTO_INCREMENT=1
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
        ) ENGINE=MyISAM DEFAULT CHARSET=latin1  COLLATE=latin1_bin AUTO_INCREMENT=1
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

          `num_nodes`     int(11)        NOT NULL,
          `num_threads`   tinyint(4)     NOT NULL,
          `queue`         varchar(32)    NOT NULL,

          `user`          varchar(32)    NOT NULL,
          `exec_path`     varchar(1024)  NOT NULL,
          `module_name`   varchar(64)            ,
          `cwd`           varchar(1024)  NOT NULL,
          PRIMARY KEY            (`run_id`),
          INDEX  `index_run_uuid` (`run_uuid`),
          INDEX `thekey` (`job_id`, `syshost`)
        ) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_bin AUTO_INCREMENT=1
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
        ) ENGINE=MyISAM DEFAULT CHARSET=latin1  COLLATE=latin1_bin AUTO_INCREMENT=1
        """)
    print("(%d) create join_run_object table" % idx); idx += 1


    # 6
    cursor.execute("""
        CREATE TABLE `xalt_env_name` (
          `env_id`        int(11)       NOT NULL auto_increment,
          `env_name`      varchar(64)   NOT NULL,
          PRIMARY KEY  (`env_id`),
          UNIQUE  KEY  `env_name` (`env_name`)
        ) ENGINE=MyISAM DEFAULT CHARSET=latin1  COLLATE=latin1_bin AUTO_INCREMENT=1
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
        ) ENGINE=MyISAM DEFAULT CHARSET=latin1  COLLATE=latin1_bin AUTO_INCREMENT=1 
        """)
    print("(%d) create join_run_env table" % idx); idx += 1

    # 8 
    cursor.execute("""
        CREATE TABLE `xalt_job_id` (
          `inc_id`        int(11)        NOT NULL auto_increment,
          `job_id`        int(11)        NOT NULL,
          PRIMARY KEY (`inc_id`)
        ) ENGINE=MyISAM DEFAULT CHARSET=latin1  COLLATE=latin1_bin AUTO_INCREMENT=1
        """)
    query = "INSERT INTO xalt_job_id VALUES(NULL, '0')"
    conn.query(query)

    cursor.close()
  except  MySQLdb.Error, e:
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)

if ( __name__ == '__main__'): main()
