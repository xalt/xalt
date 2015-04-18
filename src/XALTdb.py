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
from time       import sleep
import os, sys, re, base64
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../site")))

import MySQLdb, ConfigParser, getpass, time
import warnings
from   xalt_util     import *
from   xalt_global   import *
from   xalt_site_pkg import translate
warnings.filterwarnings("ignore", "Unknown table.*")

def convertToInt(s):
  """
  Convert to string to int.  Protect against bad input.
  @param s: Input string
  @return: integer value.  If bad return 0.
  """
  try:
    value = int(s)
  except ValueError:
    value = 0
  return value

patSQ = re.compile("'")
class XALTdb(object):
  """
  This XALTdb class opens the XALT database and is responsible for
  all the database interactions.
  """
  def __init__(self, confFn):
    """ Initialize the class and save the db config file. """
    self.__host   = None
    self.__user   = None
    self.__passwd = None
    self.__db     = None
    self.__conn   = None
    self.__confFn = confFn

  def __readFromUser(self):
    """ Ask user for database access info. (private) """

    self.__host   = raw_input("Database host:")
    self.__user   = raw_input("Database user:")
    self.__passwd = getpass.getpass("Database pass:")
    self.__db     = raw_input("Database name:")

  def __readConfig(self):
    """ Read database access info from config file. (private)"""
    confFn = self.__confFn
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

  def connect(self, databaseName = None):
    """
    Public interface to connect to DB.
    @param db:  If this exists it will be used.
    
    """
    if(os.path.exists(self.__confFn)):
      self.__readConfig()
    else:
      self.__readFromUser()

    n = 100
    for i in xrange(0,n+1):
      try:
        self.__conn = MySQLdb.connect (self.__host,self.__user,self.__passwd)
        if (databaseName):
          cursor = self.__conn.cursor()
          
          # If MySQL version < 4.1, comment out the line below
          cursor.execute("SET SQL_MODE=\"NO_AUTO_VALUE_ON_ZERO\"")
          cursor.execute("USE "+xalt.db())
        break


      except MySQLdb.Error, e:
        if (i < n):
          sleep(i*0.1)
          pass
        else:
          print ("XALTdb(%d): Error %d: %s" % (i, e.args[0], e.args[1]), file=sys.stderr)
          raise
    return self.__conn


  def db(self):
    """ Return name of db"""
    return self.__db

  def link_to_db(self, reverseMapT, linkT):
    """
    Stores the link table data into the XALT db
    @param reverseMapT: The reverse map table that maps directories to modules
    @param linkT:       The table that contains the link data.
    """
    query = ""

    try:
      conn   = self.connect()
      query  = "USE "+self.db()
      conn.query(query)
      query  = "START TRANSACTION"
      conn.query(query)
      

      query  = "SELECT uuid FROM xalt_link WHERE uuid='%s'" % linkT['uuid']
      conn.query(query)
      result = conn.store_result()
      if (result.num_rows() > 0):
        return

      build_epoch = float(linkT['build_epoch'])
      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S",
                                  time.localtime(float(linkT['build_epoch'])))

      #paranoid conversion:  Protect DB from bad input:
      exit_code = convertToInt(linkT['exit_code'])
      exec_path = patSQ.sub(r"\\'", linkT['exec_path'])


      # It is unique: lets store this link record
      query = "INSERT into xalt_link VALUES (NULL,'%s','%s','%s','%s','%s','%s','%.2f','%d','%s') " % (
        linkT['uuid'],         linkT['hash_id'],         dateTimeStr,
        linkT['link_program'], linkT['build_user'],      linkT['build_syshost'],
        build_epoch,           exit_code,                exec_path)
      conn.query(query)
      link_id = conn.insert_id()

      XALT_Stack.push("load_xalt_objects():"+linkT['exec_path'])
      self.load_objects(conn, linkT['linkA'], reverseMapT, linkT['build_syshost'],
                        "join_link_object", link_id)
      v = XALT_Stack.pop()  # unload function()
      carp("load_xalt_objects()",v)
      query = "COMMIT"
      conn.query(query)
      conn.close()

    except Exception as e:
      print(XALT_Stack.contents())
      print(query)
      print ("link_to_db(): Error ",e)
      sys.exit (1)

  def load_objects(self, conn, objA, reverseMapT, syshost, tableName, index):
    """
    Stores the objects that make an executable into the XALT DB.
    @param conn:         The db connection object
    @param objA:         The array of objects that are stored.
    @param reverseMapT:  The map between directories and modules
    @param syshost:      The system host name (stampede, darter), not login1.stampede.tacc.utexas.edu
    @param tableName:    Name of the object table.
    @param index:        The db index for the join table.
    """

    try:
      for entryA in objA:
        object_path  = entryA[0]
        hash_id      = entryA[1]
        if (hash_id == "unknown"):
          continue

        query = "SELECT obj_id, object_path FROM xalt_object WHERE hash_id='%s' AND object_path='%s' AND syshost='%s'" % (
          hash_id, object_path, syshost)
        
        conn.query(query)
        result = conn.store_result()
        if (result.num_rows() > 0):
          row    = result.fetch_row()
          obj_id = int(row[0][0])
        else:
          moduleName = obj2module(object_path, reverseMapT)
          obj_kind   = obj_type(object_path)

          query      = "INSERT into xalt_object VALUES (NULL,'%s','%s','%s',%s,NOW(),'%s') " % (
                      object_path, syshost, hash_id, moduleName, obj_kind)
          conn.query(query)
          obj_id   = conn.insert_id()
          #print("obj_id: ",obj_id, ", obj_kind: ", obj_kind,", path: ", object_path, "moduleName: ", moduleName)

        # Now link libraries to xalt_link record:
        query = "INSERT into %s VALUES (NULL,'%d','%d') " % (tableName, obj_id, index)
        conn.query(query)
  
    except Exception as e:
      print(XALT_Stack.contents())
      print(query)
      print ("load_xalt_objects(): Error %d: %s" % (e.args[0], e.args[1]))
      sys.exit (1)

  def run_to_db(self, reverseMapT, runT):
    """
    Store the "run" data into the database.
    @param: reverseMapT: The map between directories and modules
    @param: runT:        The run data stored in a table
    """
    
    nameA = [ 'num_cores', 'num_nodes', 'account', 'job_id', 'queue' , 'submit_host']
    query = ""
    try:
      conn   = self.connect()
      query  = "USE "+self.db()
      conn.query(query)
      query  = "START TRANSACTION"
      conn.query(query)

      
      translate(nameA, runT['envT'], runT['userT']);
      XALT_Stack.push("SUBMIT_HOST: "+ runT['userT']['submit_host'])

      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S",
                                  time.localtime(float(runT['userT']['start_time'])))
      uuid        = runT['xaltLinkT'].get('Build.UUID')
      if (uuid):
        uuid = "'" + uuid + "'"
      else:
        uuid = "NULL"

      #print( "Looking for run_uuid: ",runT['userT']['run_uuid'])

      query = "SELECT run_id FROM xalt_run WHERE run_uuid='%s'" % runT['userT']['run_uuid']
      conn.query(query)

      result = conn.store_result()
      if (result.num_rows() > 0):
        #print("found")
        row    = result.fetch_row()
        run_id = int(row[0][0])
        if (runT['userT']['end_time'] > 0):
          query  = "UPDATE xalt_run SET run_time='%.2f', end_time='%.2f' WHERE run_id='%d'" % (
            runT['userT']['run_time'], runT['userT']['end_time'], run_id)
          conn.query(query)
          query = "COMMIT"
          conn.query(query)
        v = XALT_Stack.pop()
        carp("SUBMIT_HOST",v)

        return
      else:
        #print("not found")
        moduleName    = obj2module(runT['userT']['exec_path'], reverseMapT)
        exit_status   = int(runT['userT'].get('exit_status',0))
        job_num_cores = int(runT['userT'].get('job_num_cores',0))
        query  = "INSERT INTO xalt_run VALUES (NULL,'%s','%s','%s', '%s',%s,'%s', '%s','%s','%.2f', '%.2f','%.2f','%d', '%d','%d','%d', '%s','%d','%s', '%s',%s,'%s') " % (
          runT['userT']['job_id'],      runT['userT']['run_uuid'],    dateTimeStr,
          runT['userT']['syshost'],     uuid,                         runT['hash_id'],
          runT['userT']['account'],     runT['userT']['exec_type'],   runT['userT']['start_time'],
          runT['userT']['end_time'],    runT['userT']['run_time'],    runT['userT']['num_cores'],
          job_num_cores,                runT['userT']['num_nodes'],   runT['userT']['num_threads'],
          runT['userT']['queue'],       exit_status,                  runT['userT']['user'],
          runT['userT']['exec_path'],   moduleName,                   runT['userT']['cwd'])
        conn.query(query)
        run_id   = conn.insert_id()

      self.load_objects(conn, runT['libA'], reverseMapT, runT['userT']['syshost'],
                        "join_run_object", run_id) 

      # loop over env. vars.
      for key in runT['envT']:
        # use the single quote pattern to protect all the single quotes in env vars.
        value = patSQ.sub(r"\\'", runT['envT'][key])
        query = "SELECT env_id FROM xalt_env_name WHERE env_name='%s'" % key
        conn.query(query)
        result = conn.store_result()
        if (result.num_rows() > 0):
          row    = result.fetch_row()
          env_id = int(row[0][0])
          found  = True
        else:
          query  = "INSERT INTO xalt_env_name VALUES(NULL, '%s')" % key
          conn.query(query)
          env_id = conn.insert_id()
          found  = False

        
        query = "INSERT INTO join_run_env VALUES (NULL, '%d', '%d', '%s')" % (
          env_id, run_id, value.encode("ascii","ignore"))
        try:
          conn.query(query)
        except Exception as e:
          query = "INSERT INTO join_run_env VALUES (NULL, '%d', '%d', '%s')" % (
            env_id, run_id, "XALT_ILLEGAL_VALUE")
          conn.query(query)
          
      v = XALT_Stack.pop()
      carp("SUBMIT_HOST",v)
      query = "COMMIT"
      conn.query(query)
      conn.close()

    except Exception as e:
      print(XALT_Stack.contents())
      print(query.encode("ascii","ignore"))
      print ("run_to_db(): ",e)
      sys.exit (1)
