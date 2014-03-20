#!/usr/bin/python

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
    print "Writing altd_db."+t+" file"
    f=open('altd_db.'+t,'w')
    config.write(f)
    f.close()
    os.chmod('altd_db.'+t,0640)
    if(os.path.exists('altd_db.conf')):
        os.remove('altd_db.conf')
    os.symlink('altd_db.'+t,"altd_db.conf")

#
# Main 
#
if(os.path.exists('altd_db.conf')):
    print "ALTD database configuration file exists!"
    q=raw_input("Do you want to use the file to fill database information?[y/n]")
    if(q.lower() == "y"):
        readConfig()    
    else:    
        readFromUser()
else:
    readFromUser()

# try connecting to the MySQL server with db information
try:
    conn = MySQLdb.connect (HOST,USER,PASSWD)
    cursor = conn.cursor()
    cursor.close()
except MySQLdb.Error, e:
    print "Error %d: %s" % (e.args[0], e.args[1])
    sys.exit (1)

#
# is new altd_db.conf file wanted
#
q=raw_input("Do you want to write a new altd_db.conf file?[y/n]")
if(q.lower() == "y"):
   writeConfig()
else:
   print "Not taking any action."
    
