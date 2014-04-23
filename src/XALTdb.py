from __future__ import print_function
import os, sys, re, base64
import MySQLdb, ConfigParser, getpass, time
import warnings
warnings.filterwarnings("ignore", "Unknown table.*")


class XALTdb(object):
  def __init__(self, confFn):
    self.__host   = None
    self.__user   = None
    self.__passwd = None
    self.__db     = None
    self.__conn   = None
    self.__confFn = confFn

  def __readFromUser(self):
    self.__host   = raw_input("Database host:")
    self.__user   = raw_input("Database user:")
    self.__passwd = getpass.getpass("Database pass:")
    self.__db     = raw_input("Database name:")

  def __readConfig(self,confFn):
    try:
      config=ConfigParser.ConfigParser()
      config.read(confFn)
      self.__host    = config.get("MYSQL","HOST")
      self.__user    = config.get("MYSQL","USER")
      self.__passwd  = base64.b64decode(config.get("MYSQL","PASSWD"))
      self.__db      = config.get("MYSQL","DB")
    except ConfigParser.NoOptionError, err:
      sys.stderr.write("\nCannot parse the config file\n")
      sys.stderr.write("Switch to user input mode...\n\n")
      self.__readFromUser()

  def connect(self, db = None):
    if(os.path.exists(self.__confFn)):
      self.__readConfig(self.__confFn)
    else:
      self.__readFromUser()

    try:
      self.__conn = MySQLdb.connect (self.__host,self.__user,self.__passwd)
      if (db):
        cursor = self.__conn.cursor()
        
        # If MySQL version < 4.1, comment out the line below
        cursor.execute("SET SQL_MODE=\"NO_AUTO_VALUE_ON_ZERO\"")
        cursor.execute("USE "+xalt.db())


    except MySQLdb.Error, e:
      print ("Error %d: %s" % (e.args[0], e.args[1]))
      sys.exit (1)

    return self.__conn


  def db(self):
    return self.__db
