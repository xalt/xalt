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
import os, sys, re, base64, json, traceback
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../site")))

import getpass, time, random, ctypes
import warnings
from   ctypes           import *   # used to interact with C shared libraries
from   xalt_util        import *
from   xalt_global      import *
from   BeautifulTbl     import BeautifulTbl

try:
  import configparser
except:
  import ConfigParser as configparser

import pymysql

try:
  input = raw_input
except:
  pass


pre_ingest_filter = False
preIngestPath = os.path.join(dirNm, "../lib64/libpreIngest.so")
if (os.path.exists(preIngestPath)): 
  libpreIngest = CDLL(os.path.realpath(os.path.join(dirNm, "../lib64/libpreIngest.so")))
  pre_ingest_filter = libpreIngest.pre_ingest_filter
  pre_ingest_filter.argtypes = [c_char_p]
  pre_ingest_filter.restype  = c_double

pkgFilter     = False
pkgFilterPath = os.path.join(dirNm, "../lib64/libpkgFilter.so")
if (os.path.exists(pkgFilterPath)): 
  libpkgFilter = CDLL(os.path.realpath(os.path.join(dirNm, "../lib64/libpkgFilter.so")))
  pkgFilter = libpkgFilter.keep_pkg
  pkgFilter.argtypes = [c_char_p]
  pkgFilter.restype  = c_int


warnings.filterwarnings("ignore", "Unknown table.*")

########################################################################
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
########################################################################

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
    self.__mpi_15_time_acc = 0.0
    self.__mpi_15_count    = 0.0
    self.__mpi_30_time_acc = 0.0
    self.__mpi_30_count    = 0.0
    self.__mpi_60_time_acc = 0.0
    self.__mpi_60_count    = 0.0
    self.__mpi_lg_time_acc = 0.0
    self.__mpi_lg_count    = 0.0
    self.__slr_15_time_acc = 0.0
    self.__slr_15_count    = 0.0
    self.__slr_30_time_acc = 0.0
    self.__slr_30_count    = 0.0
    self.__slr_60_time_acc = 0.0
    self.__slr_60_count    = 0.0
    self.__slr_lg_time_acc = 0.0
    self.__slr_lg_count    = 0.0
    
  def add(self, num_cores, runTime):
    if (num_cores > 1):
      self.__mpi_time_acc    += runTime
      self.__mpi_count       += 1.0
      if (runTime < 900.0):
        self.__mpi_15_time_acc += runTime
        self.__mpi_15_count    += 1.0
      elif (runTime < 1800.0):
        self.__mpi_30_time_acc += runTime
        self.__mpi_30_count    += 1.0
      elif (runTime < 3600.0):
        self.__mpi_60_time_acc += runTime
        self.__mpi_60_count    += 1.0
      else:
        self.__mpi_lg_time_acc += runTime
        self.__mpi_lg_count    += 1.0
    elif (runTime < 900.0):
      self.__slr_15_time_acc += runTime
      self.__slr_15_count    += 1.0
    elif (runTime < 1800.0):
      self.__slr_30_time_acc += runTime
      self.__slr_30_count    += 1.0
    elif (runTime < 3600.0):
      self.__slr_60_time_acc += runTime
      self.__slr_60_count    += 1.0
    else:
      self.__slr_lg_time_acc += runTime
      self.__slr_lg_count    += 1.0

  def print(self):
    resultA = []
    resultA.append(["Kind", "Count", "Average Time (secs)"])
    resultA.append(["----", "-----", "-------------------"])
    resultA.append(["Scalar < 15 mins",self.__slr_15_count, self.__slr_15_time_acc/max(1.0,self.__slr_15_count)])
    resultA.append(["Scalar < 30 mins",self.__slr_30_count, self.__slr_30_time_acc/max(1.0,self.__slr_30_count)])
    resultA.append(["Scalar < 60 mins",self.__slr_60_count, self.__slr_60_time_acc/max(1.0,self.__slr_60_count)])
    resultA.append(["Scalar > 60 mins",self.__slr_lg_count, self.__slr_lg_time_acc/max(1.0,self.__slr_lg_count)])
    resultA.append(["ALL MPI",         self.__mpi_count,    self.__mpi_time_acc/   max(1.0,self.__mpi_count   )])
    resultA.append(["MPI    < 15 mins",self.__mpi_15_count, self.__mpi_15_time_acc/max(1.0,self.__mpi_15_count)])
    resultA.append(["MPI    < 30 mins",self.__mpi_30_count, self.__mpi_30_time_acc/max(1.0,self.__mpi_30_count)])
    resultA.append(["MPI    < 60 mins",self.__mpi_60_count, self.__mpi_60_time_acc/max(1.0,self.__mpi_60_count)])
    resultA.append(["MPI    > 60 mins",self.__mpi_lg_count, self.__mpi_lg_time_acc/max(1.0,self.__mpi_lg_count)])

    bt = BeautifulTbl(tbl=resultA, gap = 4, justify = "lrr")
    print()
    print(bt.build_tbl(),"\n")

class XALTdb(object):
  """
  This XALTdb class opens the XALT database and is responsible for
  all the database interactions.
  """

  FAIL  = 0
  STORE = 1
  DUP   = -2
  SKIP  = -3
  
  def __init__(self, confFn):
    """ Initialize the class and save the db config file. """
    self.__host   = None
    self.__user   = None
    self.__passwd = None
    self.__db     = None
    self.__conn   = None
    self.__confFn = confFn
    self.__patt   = re.compile(r"^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$")

  def __readFromUser(self):
    """ Ask user for database access info. (private) """

    self.__host   = input("Database host:")
    self.__user   = input("Database user:")
    self.__passwd = getpass.getpass("Database pass:")
    self.__db     = input("Database name:")

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
      self.__conn = pymysql.connect(
        host     = self.__host,
        user     = self.__user,
        password = self.__passwd,
        database = self.__db,
        charset  ="utf8",
        connect_timeout=120)
    except Exception as e:
      print("Failed to open db with pymysql or mysql.connector")
      print ("XALTdb: Error: %s %s" % (e.args[0], e.args[1]))
      print(traceback.format_exc())
      raise
        
    try:
      cursor = self.__conn.cursor() # buffered=True
      
      cursor.execute("SET NAMES utf8;") #or utf8 or any other charset you want to handle
      cursor.execute("SET CHARACTER SET utf8;") #same as above
      cursor.execute("SET character_set_connection=utf8;") #same as above

    except Exception as e:
      print ("XALTdb: Error: %s %s" % (e.args[0], e.args[1]))
      print(traceback.format_exc())
      raise
    return self.__conn

  def db(self):
    """ Return name of db"""
    return self.__db

  def link_to_db(self, debug, reverseMapT, linkT):
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

      
      if (debug): sys.stdout.write("  --> Trying to connect to database\n")
      conn   = self.connect()
      cursor = conn.cursor()

      if (debug): sys.stdout.write("  --> Starting TRANSACTION\n")
      query  = 'USE '+self.db()
      cursor.execute(query)
      query  = 'START TRANSACTION'
      cursor.execute(query)
      query  = ""
      
      if (debug): sys.stdout.write("  --> Searching for build_uuid in db\n")
      uuid   = resultT['uuid'][:36]
      msg    = "my uuid is: \"" + uuid + "\""
      query  = "SELECT uuid FROM xalt_link WHERE uuid=%s"
      cursor.execute(query, [uuid])
      query  = ""
      if (cursor.rowcount > 0):
        if (debug): sys.stdout.write("  --> failed to record: build_uuid already recorded.\n\n")
        return

      build_epoch = float(resultT['build_epoch'])
      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(build_epoch))
      dateStr     = time.strftime("%Y-%m-%d",          time.localtime(build_epoch))

      exec_path   = resultT.get('exec_path',"")[:1024]  #.encode("ascii","ignore")
      link_prg    = resultT.get('link_program',"")[:64] #.encode("ascii","ignore")
      link_path   = resultT.get('link_path',"")[:1024]
      cwd         = resultT.get('wd',"UNKNOWN")[:1024]  #.encode("ascii","ignore")
      link_mname  = obj2module(link_path,reverseMapT)
      link_line   = " ".join(link_lineA)[:2048]
      
      build_user  = resultT.get('build_user',"")    #.encode("ascii","ignore")
      build_shost = resultT.get('build_syshost',"") #.encode("ascii","ignore")
      hash_id     = resultT.get('hash_id')

      #print()
      #print("RTM uuid,hash_id,date",uuid,hash_id,dateTimeStr)
      #print("RTM link_program,link_path,link_module_name", link_prg,    link_path,   link_mname)
      #print("RTM link_line: ",link_line)
      #print("RTM cwd, build_user: ",cwd, build_user)
      #print("RTM build_syshost, build_epoch:", build_shost, build_epoch)
      #print("RTM exec_path: ",exec_path)
      #print()

      #query = "INSERT into tst_link " +\
      #  "(link_id, "                  +\
      #  "uuid,hash_id,date)"          +\
      #  "VALUES (NULL, %s,%s,DATE(%s))"
      #cursor.execute(query, (uuid,        hash_id,     dateTimeStr))
      #query = "COMMIT"
      #cursor.execute(query)
                    


      if (debug): sys.stdout.write("  --> Trying to insert link record into db\n")
      # It is unique: lets store this link record
      query = "INSERT into xalt_link " +\
        "(link_id, "                                                            +\
        "uuid,hash_id,date,         link_program,link_path,link_module_name, "  +\
        "link_line,cwd,build_user,  build_syshost,build_epoch,exec_path) "      +\
        "VALUES (NULL, %s,%s,DATE(%s), %s,%s,%s, COMPRESS(%s),%s,%s, %s,%s,%s)"
      cursor.execute(query, (uuid,        hash_id,     dateTimeStr, 
                             link_prg,    link_path,   link_mname,
                             link_line,   cwd,         build_user,
                             build_shost, build_epoch, exec_path))

      if (debug): sys.stdout.write("  --> Success: link recorded\n")
      query   = ""
      link_id = cursor.lastrowid

      if (debug): sys.stdout.write("  --> Trying to insert objects into db\n")
      XALT_Stack.push("load_xalt_objects():"+resultT['exec_path'])
      self.load_objects(conn, linkA, reverseMapT, resultT['build_syshost'], dateStr,
                        "join_link_object", "link_id", link_id)
      v = XALT_Stack.pop()  # unload function()
      carp("load_xalt_objects()",v)
      
      if (debug and len(functionA) > 0): sys.stdout.write("  --> Trying to insert functions into db\n")
      # store tracked functions
      for func_name in functionA:
        query = "SELECT func_id FROM xalt_function WHERE function_name=%s"
        cursor.execute(query, [func_name[:255]])
        query = ""
        if (cursor.rowcount > 0):
          func_id = int(cursor.fetchone()[0])
        else:
          query   = "INSERT INTO xalt_function (func_id, function_name) VALUES (NULL, %s)"
          cursor.execute(query, [func_name[:255]])
          query   = ""
          func_id = cursor.lastrowid
      
        query = "INSERT INTO join_link_function (join_id, func_id, link_id, date) " +\
          "VALUES(NULL, %s, %s, %s) ON DUPLICATE KEY UPDATE func_id = %s, link_id = %s"
        cursor.execute(query, (func_id, link_id, dateStr, func_id, link_id))
        query  = ""
        
      query = "COMMIT"
      cursor.execute(query)
      query = ""
      
      conn.close()
      if (debug): sys.stdout.write("  --> Done\n\n")

    except Exception as e:
      print(XALT_Stack.contents(), file=sys.stderr)
      print(query.encode("ascii","ignore"),file=sys.stderr)
      print(msg,   file=sys.stderr)
      print("link_to_db(): Error ",e, file=sys.stderr)
      print(traceback.format_exc())
      
      sys.exit (1)

  def load_objects(self, conn, objA, reverseMapT, syshost, dateStr, tableName, idx_name, index):
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
        object_path  = entryA[0].rstrip(" \r\n")
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
                      
          query      = "INSERT into xalt_object " +\
            "(obj_id, object_path, syshost, hash_id, module_name, timestamp, lib_type)" +\
            "VALUES (NULL,%s,%s,%s,%s,NOW(),%s)"
          cursor.execute(query,(object_path, syshost, hash_id, moduleName, obj_kind))
          query      = ""
          obj_id     = cursor.lastrowid
          #print("obj_id: ",obj_id, ", obj_kind: ", obj_kind,", path: ", object_path, "moduleName: ", moduleName)

        # Now link libraries to xalt_link record:
        query = "INSERT into " + tableName + " (join_id, obj_id, " + idx_name + ", date) VALUES (NULL,%s,%s,%s) "
        cursor.execute(query,(obj_id, index, dateStr))
        query = ""
  
    except Exception as e:
      print(XALT_Stack.contents(),file=sys.stderr)
      print(query,file=sys.stderr)
      if (len(e.args) == 2):
        print ("load_xalt_objects(): Error %d: %s" % (e.args[0], e.args[1]),file=sys.stderr)
      if (len(e.args) == 1):
        print ("load_xalt_objects(): Error %s: " % (e.args[0]),file=sys.stderr)
        
      print(traceback.format_exc())
      sys.exit (1)

  def run_to_db(self, debug, reverseMapT, u2acctT, runT, timeRecord):
    """
    Store the "run" data into the database.
    @param: reverseMapT: The map between directories and modules
    @param: runT:        The run data stored in a table
    """
    
    recordMe  = False 
    if (not ('userT' in runT)):
      if (debug): sys.stdout.write("  --> failed to record: No userT in runT --> FAILURE\n\n")
      return status
    userT = runT['userT']

    exec_path = userT.get('exec_path')
    if (not exec_path):
      if (debug): sys.stdout.write("  --> failed to record: No exec_path found --> FAILURE\n\n")
      return XALTdb.FAIL

    if (not pre_ingest_filter):
      print ("failed to find: ",preIngestPath,"Exiting")
      sys.exit(1)
      
    exec_prob = pre_ingest_filter(exec_path.encode())
    prob      = random.random()
    if (prob > exec_prob):
      if (debug): sys.stdout.write("  --> Not recording due to pre-ingest filter %.4f > %.4f for: %s\n\n" % (prob, exec_prob,exec_path))
      return XALTdb.SKIP

    try:
      msg   = ""
      query = ""

      if (debug): sys.stdout.write("  --> Trying to connect to database\n")
      conn     = self.connect()
      cursor   = conn.cursor()
      query    = "USE "+self.db()
      cursor.execute(query)
      if (debug): sys.stdout.write("  --> Starting TRANSACTION\n")
      query    = "START TRANSACTION"
      cursor.execute(query)
      query    = ""

      if (not ('userDT' in runT)):
        if (debug): sys.stdout.write("  --> failed to record: No userDT in runT --> FAILURE\n\n")
        return XALTdb.FAIL
      userDT = runT['userDT']

      XALT_Stack.push("SUBMIT_HOST: "+ userT['submit_host'])
      
      cwd         = userT.get('cwd',"UNKNOWN").encode("utf-8","ignore")
      runTime     = userDT.get('run_time',0.0)
      endTime     = userDT.get('end_time',0.0)
      runTimeStr  = "%.2f" % (runTime)
      endTimeStr  = "%.2f" % (endTime)
      startTime   = userDT.get('start_time',0.0)
      if (startTime < 1):
        if (debug): sys.stdout.write("  --> failed to record: startTime epoch is < 1 second\n\n")
        v = XALT_Stack.pop()  
        return XALTdb.FAIL

      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(startTime))
      dateStr     = time.strftime("%Y-%m-%d",          time.localtime(startTime))
      xaltLinkT   = runT.get('xaltLinkT',{})
      uuid        = xaltLinkT.get('Build.UUID') or xaltLinkT.get('Build_UUID') 
      #print( "Looking for run_uuid: ",runT['userT']['run_uuid'])

      run_uuid    = userT.get('run_uuid',"UNKNOWN")[:36]
      if (run_uuid == "UNKNOWN"):
        if (debug): sys.stdout.write("  --> failed to record: run_uuid is UNKNOWN --> FAILURE\n\n")
        v = XALT_Stack.pop()  
        carp("SUBMIT_HOST",v)
        return XALTdb.FAIL
      uuid_patt = self.__patt
      m         = uuid_patt.match(run_uuid)
      if (not m):
        if (debug): sys.stdout.write("  --> failed to record: run_uuid does not match uuid pattern --> FAILURE\n\n")
        v = XALT_Stack.pop()  
        return XALTdb.FAIL


      if (debug): sys.stdout.write("  --> Searching for run_uuid in db\n")
      msg         = "my run_uuid is: \"" + run_uuid + "\""
      query       = "SELECT run_id, end_time FROM xalt_run WHERE run_uuid=%s"
      cursor.execute(query,[run_uuid])
      query       = ""
      msg         = ""
      num_cores   = userDT.get('num_cores', 1)
      num_threads = convertToTinyInt(userDT.get('num_threads',0))
      num_gpus    = convertToTinyInt(userDT.get('num_gpus',   0))

      if (debug): sys.stdout.write("  --> Trying to insert run record into db\n")
      if (cursor.rowcount > 0):
        #print("found")
        row        = cursor.fetchone()
        run_id     = int(row[0])
        my_endTime = float(row[1])

        # So either there is an end record already in the database (my_endTime > 0)
        # OR we are trying to insert a duplicate start record. (endTime is zero).
        if (my_endTime > 0 or endTime < 0.1):
          if (debug): sys.stdout.write("  --> Duplicate run_uuid, not recorded --> FAILURE\n\n")
          v = XALT_Stack.pop()  
          carp("SUBMIT_HOST",v)
          return XALTdb.DUP

        if (endTime > 0):
          query  = "UPDATE xalt_run SET run_time=%s, end_time=%s, num_threads=%s, num_gpus=%s WHERE run_id=%s" 
          cursor.execute(query,(runTimeStr, endTimeStr, num_threads, num_gpus, run_id))
          query = "COMMIT"
          cursor.execute(query)
          query = ""
          recordMe = True 
          timeRecord.add(num_cores, runTime)
          
        v = XALT_Stack.pop()
        carp("SUBMIT_HOST",v)

        if (debug): sys.stdout.write("  --> Success: updated run_time\n")
        if (debug): sys.stdout.write("  --> Done\n\n")
        return XALTdb.STORE
      else:
        #print("not found")
        moduleName    = obj2module(exec_path, reverseMapT)
        exit_status   = convertToTinyInt(userT.get('exit_status',0))
        usr_cmdline   = json.dumps(runT.get('cmdlineA'," "))[:2048]
        sum_runs      = userDT.get('sum_runs' ,   0)
        sum_times     = userDT.get('sum_times',   0.0)
        probability   = userDT.get('probability', 1.0)
        num_nodes     = userDT.get('num_nodes',   1.0)
        account       = userT.get('account')
        job_id        = userT.get('job_id')
        hash_id       = runT.get('hash_id')
        syshost       = userT.get('syshost')
        user          = userT.get('user')

        # Queue name could have a comma list.  Pick 1st one and limit it to 64 chars.
        queue         = userT.get('queue').split(',')[0][:64]
        container     = userT.get('container')
        exec_type     = userT.get('exec_type')
        
        prob_factor   = exec_prob * probability


        if ( type(container) != type(None)):
          container = container[:32]
        if ( account == "unknown"):
          account = u2acctT.get(user,"unknown")

        startTimeStr  = "%.f" % startTime
        query  = "INSERT INTO xalt_run " +\
          "(run_id, job_id, run_uuid, date, syshost, uuid, hash_id, account, exec_type, start_time, end_time, " +\
          "run_time, probability, num_cores, num_nodes, num_threads, num_gpus, queue, sum_runs, sum_time, user, " +\
          "exec_path, module_name, cwd, cmdline, container) " +\
          "VALUES (NULL, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,%s, %s,%s,COMPRESS(%s), %s)"
        cursor.execute(query, (job_id,                   run_uuid,                     dateTimeStr,
                               syshost,                  uuid,                         hash_id,
                               account,                  exec_type,                    startTimeStr,
                               endTimeStr,               runTimeStr,                   prob_factor,
                               num_cores,                num_nodes,                    num_threads,
                               num_gpus,                 queue,                        sum_runs,
                               sum_times,                user,                         exec_path,
                               moduleName,               cwd,                          usr_cmdline,
                               container))
        query    = ""
        run_id   = cursor.lastrowid
        recordMe = True 

      if (debug): sys.stdout.write("  --> Success: stored full xalt_run record\n")
      if (recordMe and endTime > 0):
        timeRecord.add(num_cores, runTime)

      if (debug): sys.stdout.write("  --> Trying to insert objects into db\n")
      self.load_objects(conn, runT['libA'], reverseMapT, runT['userT']['syshost'], dateStr,
                        "join_run_object", "run_id", run_id)

      envT    = runT['envT']

      # Not storing the total environment.
      #jsonStr = json.dumps(envT)
      #query   = "INSERT INTO xalt_total_env VALUES(NULL, %s, %s, COMPRESS(%s))"
      #cursor.execute(query, [run_id, dateStr, jsonStr])
      
      if (debug): sys.stdout.write("  --> Trying to insert env vars into db\n")
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
          query  = "INSERT INTO xalt_env_name (env_id, env_name) VALUES(NULL, %s)"
          cursor.execute(query,[key[:64]])
          query  = ""
          env_id = cursor.lastrowid
          found  = False
        
        query = "INSERT INTO join_run_env (join_id, env_id, run_id, date, env_value) VALUES (NULL, %s, %s, %s, %s)"
        cursor.execute(query,(env_id, run_id, dateStr, value))
        query = ""
          
      v = XALT_Stack.pop()
      carp("SUBMIT_HOST",v)
      query = "COMMIT"
      cursor.execute(query)
      query = ""
      conn.close()
      if (debug): sys.stdout.write("  --> Done\n\n")
      

    except Exception as e:
      if (debug): sys.stdout.write("  --> Exception\n")
      print(XALT_Stack.contents(),file=sys.stderr)
      print("query: ",query,file=sys.stderr)
      print("msg: ",msg,   file=sys.stderr)
      print("run_to_db(): ",e,file=sys.stderr)
      print("userDT: ",    userDT)
      print("userT: ",     userT)
      print("xaltLinkT: ", xaltLinkT)
      print(traceback.format_exc())
      sys.exit (1)

    return XALTdb.STORE

  def pkg_to_db(self, debug, syshost, pkgT):

    if (not pkgFilter):
      print("Failed to find: ", pkgFilterPath, "-> Exiting")
      sys.exit(1)


    keep        = 2
    skip        = 3
    program     = pkgT.get('program')[:12]
    pkg_name    = pkgT.get('package_name')[:64]
    pkg_version = pkgT.get('package_version')[:32]
    pkg_path    = pkgT.get('package_path')[:1024]
    prgmPkg     = program + ":" + pkg_name
    patternStr  = program + ":name:" + pkg_name
    name_status = pkgFilter(patternStr.encode())

    patternStr  = program + ":path:" + pkg_path
    path_status = pkgFilter(patternStr.encode())
    if (name_status == skip or path_status == skip):
      if (debug): sys.stdout.write("  --> Failed to record: pkgFilter blocks recording of \""+prgmPkg+"\"\n")
      return

    try:
      conn   = self.connect()
      cursor = conn.cursor()
      query  = "USE "+self.db()
      cursor.execute(query)
      query  = "START TRANSACTION"
      cursor.execute(query)
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
        query       = "INSERT into xalt_pkg (pkg_id, run_id, program, pkg_name, pkg_version, pkg_path) " +\
          "VALUES(NULL,%s,%s,%s,%s,%s)"
        cursor.execute(query,(run_id, program, pkg_name, pkg_version, pkg_path))
        query       = ""
        if (debug): sys.stdout.write("  --> Success: pkg entry \""+prgmPkg+"\" stored\n")
      else:
        if (debug): sys.stdout.write("  --> Failed to record: No run_uuid to connect packages to for \""+prgmPkg+"\"\n")
        
      v = XALT_Stack.pop()
      carp("SYSHOST",v)
      query = "COMMIT"
      cursor.execute(query)
      query = ""
      conn.close()

    except Exception as e:
      print(XALT_Stack.contents(),file=sys.stderr)
      print(query.encode("ascii","ignore"),file=sys.stderr)
      print ("pkg_to_db(): ",e,file=sys.stderr)
      print(traceback.format_exc())
      sys.exit (1)
