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
import os, sys, re, base64, json, traceback
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../site")))

import MySQLdb, getpass, time
import warnings
from   xalt_util        import *
from   xalt_global      import *
from   BeautifulTbl     import BeautifulTbl

try:
  import configparser
except:
  import ConfigParser as configparser

warnings.filterwarnings("ignore", "Unknown table.*")

import inspect

def __LINE__():
    try:
        raise Exception
    except:
        return sys.exc_info()[2].tb_frame.f_back.f_lineno

def __FILE__():
    fnA = os.path.split(inspect.currentframe().f_code.co_filename)
    return fnA[1]

#print ("file: '%s', line: %d" % (__FILE__(), __LINE__()), file=sys.stderr)

def convertToTinyInt(s):
  """
  Convert to string to int.  Protect against bad input.
  @param s: Input string
  @return: integer value.  If bad return 0.
  """
  try:
    value = int(s)
    if  (value > 127):
      value = 127
    elif(value < -128):
      value = -128
  except ValueError as e:
    value = 0
  return value

class TimeRecord(object):
  def __init__(self):
    self.__mpi_time_acc    = 0.0
    self.__mpi_count       = 0.0
    self.__slr_05_time_acc = 0.0
    self.__slr_05_count    = 0.0
    self.__slr_10_time_acc = 0.0
    self.__slr_10_count    = 0.0
    self.__slr_20_time_acc = 0.0
    self.__slr_20_count    = 0.0
    self.__slr_lg_time_acc = 0.0
    self.__slr_lg_count    = 0.0
    
  def add(self, num_cores, runTime):
    if (num_cores > 1):
      self.__mpi_time_acc    += runTime
      self.__mpi_count       += 1.0
    elif (runTime < 300.0):
      self.__slr_05_time_acc += runTime
      self.__slr_05_count    += 1.0
    elif (runTime < 600.0):
      self.__slr_10_time_acc += runTime
      self.__slr_10_count    += 1.0
    elif (runTime < 1200.0):
      self.__slr_20_time_acc += runTime
      self.__slr_20_count    += 1.0
    else:
      self.__slr_lg_time_acc += runTime
      self.__slr_lg_count    += 1.0

  def print(self):
    resultA = []
    resultA.append(["Kind", "Count", "Average Time"])
    resultA.append(["----", "-----", "------------"])
    resultA.append(["Scalar <  5 mins",self.__slr_05_count, self.__slr_05_time_acc/max(1.0,self.__slr_05_count)])
    resultA.append(["Scalar < 10 mins",self.__slr_10_count, self.__slr_10_time_acc/max(1.0,self.__slr_10_count)])
    resultA.append(["Scalar < 20 mins",self.__slr_20_count, self.__slr_20_time_acc/max(1.0,self.__slr_20_count)])
    resultA.append(["Scalar > 20 mins",self.__slr_lg_count, self.__slr_lg_time_acc/max(1.0,self.__slr_lg_count)])
    resultA.append(["MPI",             self.__mpi_count,    self.__mpi_time_acc/   max(1.0,self.__mpi_count   )])

    bt = BeautifulTbl(tbl=resultA, gap = 4, justify = "lrr")
    print()
    print(bt.build_tbl(),"\n")

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
      config=configparser.ConfigParser()
      config.read(confFn)
      self.__host    = config.get("MYSQL","HOST")
      self.__user    = config.get("MYSQL","USER")
      self.__passwd  = base64.b64decode(config.get("MYSQL","PASSWD")).decode()
      self.__db      = config.get("MYSQL","DB")
    except configparser.NoOptionError as err:
      sys.stderr.write("\nCannot parse the config file\n")
      sys.stderr.write("Switch to user input mode...\n\n")
      print(traceback.format_exc())
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

    try:
      self.__conn = MySQLdb.connect \
                      (self.__host,self.__user,self.__passwd, use_unicode=True, \
                       charset="utf8", connect_timeout=120)
      if (databaseName):
        cursor = self.__conn.cursor()
        
        # If MySQL version < 4.1, comment out the line below
        cursor.execute("SET SQL_MODE=\"NO_AUTO_VALUE_ON_ZERO\"")
        cursor.execute("USE "+xalt.db())

        self.__conn.set_character_set('utf8')
        cursor.execute("SET NAMES utf8;") #or utf8 or any other charset you want to handle
        cursor.execute("SET CHARACTER SET utf8;") #same as above
        cursor.execute("SET character_set_connection=utf8;") #same as above

    except MySQLdb.Error as e:
      print ("XALTdb: Error: %s %s" % (e.args[0], e.args[1]))
      print(traceback.format_exc())
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

    msg = "no message"
    try:
      link_lineA = linkT.get('link_line',[])
      linkA      = linkT.get('linkA',[])
      functionA  = linkT.get('function',[])
      resultT    = linkT.get("resultT")
      if (resultT == None):
        resultT    = linkT

      
      conn   = self.connect()
      cursor = conn.cursor()
      query  = 'USE '+self.db()
      conn.query(query)
      query  = 'START TRANSACTION'
      conn.query(query)
      query  = ""
      
      uuid   = resultT['uuid'][:36]
      msg    = "my uuid is: \"" + uuid + "\""
      query  = "SELECT uuid FROM xalt_link WHERE uuid=%s"
      cursor.execute(query, [uuid])
      query  = ""
      if (cursor.rowcount > 0):
        return

      build_epoch = float(resultT['build_epoch'])
      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(float(resultT['build_epoch'])))
      dateStr     = time.strftime("%Y-%m-%d",          time.localtime(float(resultT['build_epoch'])))

      exec_path   = resultT['exec_path'][:1024]
      link_prg    = resultT['link_program'][:64]
      link_path   = resultT['link_path']
      cwd         = resultT.get('wd',"UNKNOWN")[:1024]
      link_mname  = obj2module(link_path,reverseMapT)
      link_line   = json.dumps(link_lineA)
      build_user  = resultT['build_user']
      build_shost = resultT['build_syshost']
      hash_id     = resultT['hash_id']

      # It is unique: lets store this link record
      query = "INSERT into xalt_link VALUES (NULL, %s,%s,%s, %s,%s,%s, COMPRESS(%s),%s,%s, %s,%s,%s)"
      cursor.execute(query, (uuid,        hash_id,     dateTimeStr, 
                             link_prg,    link_path,   link_mname,
                             link_line,   cwd,         build_user,
                             build_shost, build_epoch, exec_path))

      query   = ""
      link_id = cursor.lastrowid

      XALT_Stack.push("load_xalt_objects():"+resultT['exec_path'])
      self.load_objects(conn, linkA, reverseMapT, resultT['build_syshost'], dateStr,
                        "join_link_object", link_id)
      v = XALT_Stack.pop()  # unload function()
      carp("load_xalt_objects()",v)
      
      # store tracked functions
      for func_name in functionA:
        query = "SELECT func_id FROM xalt_function WHERE function_name=%s"
        cursor.execute(query, [func_name[:255]])
        query = ""
        if (cursor.rowcount > 0):
          func_id = int(cursor.fetchone()[0])
        else:
          query   = "INSERT INTO xalt_function VALUES (NULL, %s)"
          cursor.execute(query, [func_name[:255]])
          query   = ""
          func_id = cursor.lastrowid
      
        query = "INSERT INTO join_link_function VALUES(NULL, %s, %s, %s) \
                     ON DUPLICATE KEY UPDATE func_id = %s, link_id = %s"
        cursor.execute(query, (func_id, link_id, dateStr, func_id, link_id))
        query  = ""
        
      query = "COMMIT"
      conn.query(query)
      query = ""
      
      conn.close()

    except Exception as e:
      print(XALT_Stack.contents(), file=sys.stderr)
      print(query.encode("ascii","ignore"),file=sys.stderr)
      print(msg,   file=sys.stderr)
      print("link_to_db(): Error ",e, file=sys.stderr)
      print(traceback.format_exc())
      
      sys.exit (1)

  def load_objects(self, conn, objA, reverseMapT, syshost, dateStr, tableName, index):
    """
    Stores the objects that make an executable into the XALT DB.
    @param conn:         The db connection object
    @param objA:         The array of objects that are stored.
    @param reverseMapT:  The map between directories and modules
    @param syshost:      The system host name (stampede, darter), not login1.stampede.tacc.utexas.edu
    @param tableName:    Name of the object table.
    @param index:        The db index for the join table.
    """

    cursor = conn.cursor()
    try:
      for entryA in objA:
        object_path  = entryA[0]
        hash_id      = entryA[1]
        if (hash_id == "unknown"):
          continue

        query = "SELECT obj_id, object_path FROM xalt_object WHERE hash_id=%s AND object_path=%s AND syshost=%s"
          
        cursor.execute(query,(hash_id, object_path[:1024], syshost[:64]))
        query = ""

        if (cursor.rowcount > 0):
          row    = cursor.fetchone()
          obj_id = int(row[0])
        else:
          moduleName = obj2module(object_path, reverseMapT)
          obj_kind   = obj_type(object_path)

          
          if (moduleName and len(moduleName) > 64):
            moduleName = moduleName[:63]
                      
          query      = "INSERT into xalt_object VALUES (NULL,%s,%s,%s,%s,NOW(),%s)"
          cursor.execute(query,(object_path, syshost, hash_id, moduleName, obj_kind))
          query      = ""
          obj_id   = conn.insert_id()
          #print("obj_id: ",obj_id, ", obj_kind: ", obj_kind,", path: ", object_path, "moduleName: ", moduleName)

        # Now link libraries to xalt_link record:
        query = "INSERT into " + tableName + " VALUES (NULL,%s,%s,%s) "
        cursor.execute(query,(obj_id, index, dateStr))
        query = ""
  
    except Exception as e:
      print(XALT_Stack.contents(),file=sys.stderr)
      print(query,file=sys.stderr)
      print ("load_xalt_objects(): Error %d: %s" % (e.args[0], e.args[1]),file=sys.stderr)
      print(traceback.format_exc())
      sys.exit (1)

  def run_to_db(self, reverseMapT, u2acctT, runT, timeRecord):
    """
    Store the "run" data into the database.
    @param: reverseMapT: The map between directories and modules
    @param: runT:        The run data stored in a table
    """
    
    msg   = ""
    query = ""
    try:
      conn   = self.connect()
      cursor = conn.cursor()
      query  = "USE "+self.db()
      conn.query(query)
      query  = "START TRANSACTION"
      conn.query(query)
      query  = ""

      XALT_Stack.push("SUBMIT_HOST: "+ runT['userT']['submit_host'])

      runTime     = "%.2f" % (runT['userDT']['run_time'])
      endTime     = "%.2f" % (runT['userDT']['end_time'])
      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(float(runT['userDT']['start_time'])))
      dateStr     = time.strftime("%Y-%m-%d", time.localtime(float(runT['userDT']['start_time'])))
      xaltLinkT   = runT['xaltLinkT']
      uuid        = xaltLinkT.get('Build.UUID') or xaltLinkT.get('Build_UUID') 
      #print( "Looking for run_uuid: ",runT['userT']['run_uuid'])

      run_uuid    = runT['userT']['run_uuid'][:36]
      msg         = "my run_uuid is: \"" + run_uuid + "\""
      query       = "SELECT run_id FROM xalt_run WHERE run_uuid=%s"
      cursor.execute(query,[run_uuid])
      query       = ""
      msg         = ""
      num_cores   = runT['userDT']['num_cores']
      num_threads = convertToTinyInt(runT['userDT'].get('num_threads',0))
      num_gpus    = convertToTinyInt(runT['userDT'].get('num_gpus',0))
      stored      = False 
      recordMe    = False 

      if (cursor.rowcount > 0):
        #print("found")
        row    = cursor.fetchone()
        run_id = int(row[0])
        if (runT['userDT']['end_time'] > 0):
          query  = "UPDATE xalt_run SET run_time=%s, end_time=%s, num_threads=%s, num_gpus=%s WHERE run_id=%s" 
          cursor.execute(query,(runTime, endTime, num_threads, num_gpus, run_id))
          query = "COMMIT"
          conn.query(query)
          query = ""
          recordMe = True 
        v = XALT_Stack.pop()
        carp("SUBMIT_HOST",v)

        return
      else:
        #print("not found")
        moduleName    = obj2module(runT['userT']['exec_path'], reverseMapT)
        exit_status   = convertToTinyInt(runT['userT'].get('exit_status',0))
        usr_cmdline   = json.dumps(runT['cmdlineA'])
        sum_runs      = runT['userDT'].get('sum_runs' ,   0)
        sum_times     = runT['userDT'].get('sum_times',   0.0)
        probability   = runT['userDT'].get('probability', 1.0)
        account       = runT['userT']['account']
        user          = runT['userT']['user']
        if ( account == "unknown"):
          account = u2acctT.get(user,"unknown")


        startTime     = "%.f" % float(runT['userDT']['start_time'])
        query  = "INSERT INTO xalt_run VALUES (NULL, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,COMPRESS(%s))"
        cursor.execute(query, (runT['userT']['job_id'],  run_uuid,                     dateTimeStr,
                               runT['userT']['syshost'], uuid,                         runT['hash_id'],
                               account,                  runT['userT']['exec_type'],   startTime,
                               endTime,                  runTime,                      probability,
                               num_cores,                runT['userDT']['num_nodes'],  num_threads,
                               num_gpus,                 runT['userT']['queue'],       sum_runs,
                               sum_times,                user,                         runT['userT']['exec_path'],
                               moduleName,               runT['userT']['cwd'],         usr_cmdline))
        query    = ""
        run_id   = cursor.lastrowid
        stored   = True
        recordMe = True 


      if (recordMe and float(runTime) > 0.0):
        timeRecord.add(num_cores,float(runTime))


      self.load_objects(conn, runT['libA'], reverseMapT, runT['userT']['syshost'], dateStr,
                        "join_run_object", run_id)

      envT    = runT['envT']

      # Not storing the total environment.
      #jsonStr = json.dumps(envT)
      #query   = "INSERT INTO xalt_total_env VALUES(NULL, %s, %s, COMPRESS(%s))"
      #cursor.execute(query, [run_id, dateStr, jsonStr])
      
      # loop over env. vars.
      for key in envT:

        value = envT[key][:65535]
        query = "SELECT env_id FROM xalt_env_name WHERE env_name=%s"
        cursor.execute(query,[key[:64]])
        query = ""
        if (cursor.rowcount > 0):
          row    = cursor.fetchone()
          env_id = int(row[0])
          found  = True
        else:
          query  = "INSERT INTO xalt_env_name VALUES(NULL, %s)"
          cursor.execute(query,[key[:64]])
          query  = ""
          env_id = cursor.lastrowid
          found  = False
        
        query = "INSERT INTO join_run_env VALUES (NULL, %s, %s, %s, %s)"
        cursor.execute(query,(env_id, run_id, dateStr, value))
        query = ""
          
      v = XALT_Stack.pop()
      carp("SUBMIT_HOST",v)
      query = "COMMIT"
      conn.query(query)
      query = ""
      conn.close()

    except Exception as e:
      print(XALT_Stack.contents(),file=sys.stderr)
      print("query: ",query,file=sys.stderr)
      print("msg: ",msg,   file=sys.stderr)
      print("run_to_db(): ",e,file=sys.stderr)
      print(traceback.format_exc())
      sys.exit (1)

    return stored

  def pkg_to_db(self, syshost, pkgT):

    try:
      conn   = self.connect()
      cursor = conn.cursor()
      query  = "USE "+self.db()
      conn.query(query)
      query  = "START TRANSACTION"
      conn.query(query)
      query  = ""

      XALT_Stack.push("SYSHOST: "+syshost)

      query    = "SELECT run_id FROM xalt_run WHERE run_uuid=%s"
      run_uuid = pkgT['xalt_run_uuid'][:36]
      cursor.execute(query,[run_uuid])
      query    = ""
      #print ("run_uuid: '%s', rowcount: %d" % (pkgT['xalt_run_uuid'], cursor.rowcount), file=sys.stderr)

      if (cursor.rowcount > 0):
        row         = cursor.fetchone()
        run_id      = int(row[0])
        program     = pkgT['program'][:12]
        pkg_name    = pkgT['package_name'][:64]
        pkg_version = pkgT['package_version'][:32]
        pkg_path    = pkgT['package_path'][:10]
        
        query  = "INSERT into xalt_pkg VALUES(NULL,%s,%s,%s,%s,%s)"
        cursor.execute(query,(run_id, program, pkg_name, pkg_version, pkg_path))
        query  = ""
        
      v = XALT_Stack.pop()
      carp("SYSHOST",v)
      query = "COMMIT"
      conn.query(query)
      query = ""
      conn.close()

    except Exception as e:
      print(XALT_Stack.contents(),file=sys.stderr)
      print(query.encode("ascii","ignore"),file=sys.stderr)
      print ("pkg_to_db(): ",e,file=sys.stderr)
      print(traceback.format_exc())
      sys.exit (1)
