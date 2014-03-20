#!/usr/bin/python -E

import sys,os
import MySQLdb
import ConfigParser
import base64
import time
import getpass

def readFromUser():
    global HOST,USER,PASSWD,DB
    HOST=raw_input("Database host:")
    USER=raw_input("Database user:")
    PASSWD=getpass.getpass("Database pass:")
    DB=raw_input("Database name:")

def readConfig():
    try:
        global HOST,USER,PASSWD,DB
        config=ConfigParser.ConfigParser()
        config.read("altd_db.conf")
        HOST=config.get("MYSQL","HOST")
        USER=config.get("MYSQL","USER")
        PASSWD=base64.b64decode(config.get("MYSQL","PASSWD"))
        DB=config.get("MYSQL","DB")
    except ConfigParser.NoOptionError, err:
        sys.stderr.write("\nCannot parse the config file\n")
        sys.stderr.write("Switch to user input mode...\n\n")
        readFromUser()

def writeConfig():
    config=ConfigParser.ConfigParser()
    config.add_section("MYSQL")
    config.set("MYSQL","HOST",HOST)
    config.set("MYSQL","USER",USER)
    config.set("MYSQL","PASSWD",base64.b64encode(PASSWD))
    config.set("MYSQL","DB",DB)
    
    t=time.strftime("%m%d%H%M%Y")
    f=open('altd_db.'+t,'w')
    config.write(f)
    f.close()
    os.chmod('altd_db.'+t,0640)
    if(os.path.exists('altd_db.conf')):
        os.remove('altd_db.conf')
    os.symlink('altd_db.'+t,"altd_db.conf")

if(os.path.exists('altd_db.conf')):
    print "ALTD database configuration file exists!"
    q=raw_input("Do you want to use the file to fill database information?[y/n]")
    if(q.lower() == "y"):
        readConfig()    
    else:    
        readFromUser()
else:
    readFromUser()

MACHINE=raw_input("Machine name:")

# connect to the MySQL server
try:
    conn = MySQLdb.connect (HOST,USER,PASSWD)
except MySQLdb.Error, e:
    print "Error %d: %s" % (e.args[0], e.args[1])
    sys.exit (1)

# create database and related tables
try:
    cursor = conn.cursor()

    # If MySQL version < 4.1, comment out the line below
    cursor.execute("SET SQL_MODE=\"NO_AUTO_VALUE_ON_ZERO\"")
    # If the database does not exist, create it, otherwise, switch to the database.
    cursor.execute("CREATE DATABASE IF NOT EXISTS %s DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci" % DB)
    cursor.execute("USE "+DB)
     
    
    # Table structure for table `altd_<MACHINE>_jobs`
    cursor.execute("""
        CREATE TABLE `altd_%s_jobs` (
          `run_inc` int(11) NOT NULL auto_increment,
          `tag_id` int(11) NOT NULL,
          `executable` varchar(1024) NOT NULL,
          `username` varchar(64) NOT NULL,
          `run_date` date NOT NULL,
          `job_launch_id` int(11) NOT NULL,
          `build_machine` varchar(64) NOT NULL,
          PRIMARY KEY  (`run_inc`)
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1
    """ % MACHINE)

    # Table structure for table `altd_<MACHINE>_link_tags`
    cursor.execute("""
        CREATE TABLE `altd_%s_link_tags` (
          `tag_id` int(11) NOT NULL auto_increment,
          `linkline_id` int(11) NOT NULL,
          `username` varchar(64) NOT NULL,
          `exit_code` tinyint(4) NOT NULL,
          `link_date` date NOT NULL,
          PRIMARY KEY  (`tag_id`)
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 
    """ % MACHINE)

    # Table structure for table `altd_<MACHINE>_linkline`
    cursor.execute("""
        CREATE TABLE `altd_%s_linkline` (
          `linking_inc` int(11) NOT NULL auto_increment,
          `linkline` varchar(4096) NOT NULL,
          PRIMARY KEY  (`linking_inc`)
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 
    """ % MACHINE)    
    
    cursor.close()

    writeConfig()
    
    
except MySQLdb.Error, e:
    print "Error %d: %s" % (e.args[0], e.args[1])
    sys.exit (1)

