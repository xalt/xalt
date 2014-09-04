#!/usr/bin/env python
# -*- python -*-
#
#  optional input:
#    XALT_USERS:  colon separated list of users
#
#
# Git Version: @git@

from __future__  import print_function
import os, sys, re, MySQLdb, json, time, argparse
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from xalt_stack    import Stack
from XALTdb        import XALTdb
from xalt_site_pkg import translate
from xalt_util     import files_in_tree, capture, config_logger
from progressBar   import ProgressBar
import warnings, getent
warnings.filterwarnings("ignore", "Unknown table.*")

ConfigBaseNm = "xalt_db"
ConfigFn     = ConfigBaseNm + ".conf"
logger       = config_logger()
patSQ        = re.compile("'")
pstack       = Stack()

class CmdLineOptions(object):
  def __init__(self):
    pass
  
  def execute(self):
    parser = argparse.ArgumentParser()
    parser.add_argument("--delete",      dest='delete', action="store_true", help="delete files after reading")
    parser.add_argument("--timer",       dest='timer',  action="store_true", help="Time runtime")
    parser.add_argument("--reverseMapD", dest='rmapD',  action="store",      help="Path to the directory containing the json reverseMap")
    args = parser.parse_args()
    return args


def remove_files(fileA):
  for f in fileA:
    os.remove(f)


def passwd_generator():
  xaltUserA = os.environ.get("XALT_USERS")
  if (xaltUserA):
    for user in xaltUserA.split(":"):
      yield user, os.path.expanduser("~" + user)

  else:
    for entry in getent.passwd():
      yield entry.name, entry.dir

numberPat = re.compile(r'[0-9][0-9]*')
def obj_type(object_path):
  result = None
  a      = object_path.split('.')
  for entry in reversed(a):
    m = numberPat.search(entry)
    if (m):
      continue
    else:
      result = entry
      break
  return result

defaultPat = re.compile(r'default:?')
def obj2module(object_path, reverseMapT):
  dirNm, fn  = os.path.split(object_path)
  moduleName = 'NULL'
  pkg         = reverseMapT.get(dirNm)
  if (pkg):
    flavor    = pkg['flavor'][0]
    flavor    = defaultPat.sub('',flavor)
    if (flavor):
      moduleName = "'" + pkg['pkg'] + '(' + flavor + ")'"
    else:
      moduleName = "'" + pkg['pkg'] + "'"
  return moduleName

def link_json_to_db(xalt, user, reverseMapT, linkFnA):

  num = 0
  query = ""

  try:
    for fn in linkFnA:
      pstack.push("fn: "+fn)
      num  += 1
      f     = open(fn,"r")
      try:
        linkT = json.loads(f.read())
      except:  
        f.close()
        continue

      f.close()
      conn   = xalt.connect()
      query  = "USE "+xalt.db()
      conn.query(query)
      query  = "SELECT uuid FROM xalt_link WHERE uuid='%s'" % linkT['uuid']
      conn.query(query)
      result = conn.store_result()
      if (result.num_rows() > 0):
        continue

      build_epoch = float(linkT['build_epoch'])
      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S",
                                  time.localtime(float(linkT['build_epoch'])))
      # It is unique: lets store this link record
      query = "INSERT into xalt_link VALUES (NULL,'%s','%s','%s','%s','%s','%s','%.2f','%d','%s') " % (
        linkT['uuid'],         linkT['hash_id'],         dateTimeStr,
        linkT['link_program'], linkT['build_user'],      linkT['build_syshost'],
        build_epoch,           int(linkT['exit_code']), linkT['exec_path'])
      conn.query(query)
      link_id = conn.insert_id()
      #print("link_id: ",link_id)

      pstack.push("load_xalt_objects()")
      load_xalt_objects(conn, linkT['linkA'], reverseMapT, linkT['build_syshost'],
                        "join_link_object", link_id)
      pstack.pop()
      pstack.pop()

  except Exception as e:
    print(pstack.contents())
    print(query)
    print ("link_json_to_db(): Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)
  return num

def load_xalt_objects(conn, objA, reverseMapT, syshost, table, index):

  query = ""
  try:
    for entryA in objA:
      object_path  = entryA[0]
      hash_id      = entryA[1]
      if (hash_id == "unknown"):
        continue

      query = "SELECT obj_id FROM xalt_object WHERE hash_id='%s' AND object_path='%s' AND syshost='%s'" % (
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
      query = "INSERT into %s VALUES (NULL,'%d','%d') " % (table, obj_id, index)
      conn.query(query)


  except Exception as e:
    print(pstack.contents())
    print(query)
    print ("load_xalt_objects(): Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)


def run_json_to_db(xalt, user, reverseMapT, runFnA):
  nameA = [ 'num_cores', 'num_nodes', 'account', 'job_id', 'queue', 'submit_host' ]
  num   = 0
  query = ""
  try:
    for fn in runFnA:
      pstack.push("fn: "+fn)
      num   += 1
      f      = open(fn,"r")
      
      try:
        runT   = json.loads(f.read())
      except:
        f.close()
        continue
      f.close()
      conn   = xalt.connect()
      query  = "USE "+xalt.db()
      conn.query(query)

      translate(nameA, runT['envT'], runT['userT']);
      pstack.push("SUBMIT_HOST: "+ runT['userT']['submit_host'])

      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S",
                                  time.localtime(float(runT['userT']['start_time'])))
      uuid        = runT['xaltLinkT'].get('Build.UUID')
      if (uuid):
        uuid = "'" + uuid + "'"
      else:
        uuid = "NULL"

      #print( "Looking for run_uuid: ",runT['userT']['run_uuid'])

      pquery = "SELECT run_id FROM xalt_run WHERE run_uuid='%s'" % runT['userT']['run_uuid']
      conn.query(query)

      result = conn.store_result()
      if (result.num_rows() > 0):
        #print("found")
        row    = result.fetch_row()
        run_id = int(row[0][0])
        query  = "UPDATE xalt_run SET run_time='%.2f', end_time='%.2f' WHERE run_id='%d'" % (
          runT['userT']['run_time'], runT['userT']['end_time'], run_id)
        conn.query(query)
        continue
      else:
        #print("not found")
        moduleName = obj2module(runT['userT']['exec_path'], reverseMapT)
        query  = "INSERT INTO xalt_run VALUES (NULL,'%s','%s','%s', '%s',%s,'%s', '%s','%s','%.2f', '%.2f','%.2f','%d', '%d','%d','%s', '%s','%s',%s,'%s') " % (
          runT['userT']['job_id'],      runT['userT']['run_uuid'],    dateTimeStr,
          runT['userT']['syshost'],     uuid,                         runT['hash_id'],
          runT['userT']['account'],     runT['userT']['exec_type'],   runT['userT']['start_time'],
          runT['userT']['end_time'],    runT['userT']['run_time'],    runT['userT']['num_cores'],
          runT['userT']['num_nodes'],   runT['userT']['num_threads'], runT['userT']['queue'],
          runT['userT']['user'],        runT['userT']['exec_path'],   moduleName,
          runT['userT']['cwd'])
        conn.query(query)
        run_id   = conn.insert_id()

      load_xalt_objects(conn, runT['libA'], reverseMapT, runT['userT']['syshost'],
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
        #print("env_id: ", env_id, ", found: ",found)

        
        query = "INSERT INTO join_run_env VALUES (NULL, '%d', '%d', '%s')" % (
          env_id, run_id, value)
                   #value.encode("ascii","ignore"))
        conn.query(query)
      pstack.pop()  
      pstack.pop()  


  except Exception as e:
    print(pstack.contents())
    print(query)
    print ("run_json_to_db(): Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)
  return num

class Rmap(object):
  def __init__(self, rmapD):
    self.__rmapT = {}
    if (not rmapD):
      return
    rmapA   = rmapD.split(':')
    searchA = [ ".json", ".old.json"]
    rmapFn  = None
    for dir in rmapA:
      for ext in searchA:
        rmapFn = os.path.join(dir, "jsonReverseMapT" + ext)
        if (os.access(rmapFn, os.R_OK)):
          break
    if (rmapFn):
      rmpMtime = os.stat(rmapFn).st_mtime
      f        = open(rmapFn,"r")
      t        = json.loads(f.read())
      f.close()
      tsFn     = t.get('timestampFn')
      if (tsFn):
        tsMtime = os.stat(tsFn).st_mtime
        if (rmpMtime >= tsMtime):
          self.__rmapT = t['reverseMapT']
      else:
        self.__rmapT = t['reverseMapT']
        

  def reverseMapT(self):
    return self.__rmapT 



def main():
  # push User, host and command line on to pstack
  sA = []
  sA.append("CommandLine:")
  for v in sys.argv:
    sA.append('"'+v+'"')

  args   = CmdLineOptions().execute()
  xalt   = XALTdb(ConfigFn)

  num    = int(capture("getent passwd | wc -l"))
  pbar   = ProgressBar(maxVal=num)
  icnt   = 0

  t1     = time.time()

  rmapT  = Rmap(args.rmapD).reverseMapT()

  iuser  = 0
  lnkCnt = 0
  runCnt = 0

  for user, hdir in passwd_generator():
    pstack.push("User: " + user)
    xaltDir = os.path.join(hdir,".xalt.d")
    if (os.path.isdir(xaltDir)):
      iuser   += 1
      linkFnA  = files_in_tree(xaltDir, "*/link.*.json")
      pstack.push("link_json_to_db()")
      lnkCnt  += link_json_to_db(xalt, user, rmapT, linkFnA)
      pstack.pop()
      if (args.delete):
        remove_files(linkFnA)
        remove_files(files_in_tree(xaltDir, "*/.link.*.json"))

      runFnA   = files_in_tree(xaltDir, "*/run.*.json")
      pstack.push("run_json_to_db()")
      runCnt  += run_json_to_db(xalt, user, rmapT, runFnA)
      pstack.pop()
      if (args.delete):
        remove_files(runFnA)
        remove_files(files_in_tree(xaltDir, "*/.run.*.json"))
    icnt += 1
    pstack.pop()
    pbar.update(icnt)

  xalt.connect().close()
  pbar.fini()
  t2 = time.time()
  rt = t2 - t1
  if (args.timer):
    print("Time: ", time.strftime("%T", time.gmtime(rt)))

  print("num users: ", iuser, ", num links: ", lnkCnt, ", num runs: ", runCnt)

if ( __name__ == '__main__'): main()
