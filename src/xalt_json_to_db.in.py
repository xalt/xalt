#!/usr/bin/env python
# -*- python -*-
from __future__  import print_function
import os, sys, re, MySQLdb, json, time, argparse
dirNm, execName = os.path.split(sys.argv[0])
sys.path.append(os.path.abspath(os.path.join(dirNm, "../libexec")))
sys.path.append(os.path.abspath(os.path.join(dirNm, "../site")))

from XALTdb      import XALTdb
from SitePkg     import translate
from util        import files_in_tree, capture
from progressBar import ProgressBar
import warnings
warnings.filterwarnings("ignore", "Unknown table.*")

ConfigBaseNm = "xalt_db"
ConfigFn     = ConfigBaseNm + ".conf"

class CmdLineOptions(object):
  def __init__(self):
    pass
  
  def execute(self):
    parser = argparse.ArgumentParser()
    parser.add_argument("--delete",  dest='delete', action="store_true", help="delete files after reading")
    parser.add_argument("--timer",   dest='timer',  action="store_true", help="Time runtime")
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
    passwd = open("/etc/passwd","r")
    for line in passwd:
      (username, encrypwd, uid, gid, gecos, homedir, usershell) = line.split(':') 
      yield username, homedir
    passwd.close()

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


def link_json_to_db(xalt, user, linkFnA):

  try:
    for fn in linkFnA:
      f     = open(fn,"r")
      linkT = json.loads(f.read())
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

      for entryA in linkT['linkA']:
        object_path = entryA[0]
        hash_id     = entryA[1]

        query = "SELECT obj_id FROM xalt_object WHERE hash_id='%s' AND object_path='%s' AND syshost='%s'" % (
          hash_id, object_path, linkT['build_syshost'])
        
        conn.query(query)
        result = conn.store_result()
        if (result.num_rows() > 0):
          row    = result.fetch_row()
          obj_id = int(row[0][0])
          #print("found old obj_id: ",obj_id)
        else:
          obj_kind = obj_type(object_path)

          query    = "INSERT into xalt_object VALUES (NULL,'%s','%s','%s',NOW(),'%s') " % (
                      object_path, linkT['build_syshost'], hash_id, obj_kind)
          conn.query(query)
          obj_id   = conn.insert_id()
          #print("obj_id: ",obj_id, ", obj_kind: ", obj_kind,", path: ", object_path)

        # Now link libraries to xalt_link record:
        query = "INSERT into join_link_object VALUES (NULL,'%d','%d') " % (obj_id, link_id)
        conn.query(query)


  except MySQLdb.Error, e:
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)
    



def run_json_to_db(xalt, user, runFnA):
  nameA = [ 'num_cores', 'num_nodes', 'account', 'job_id', 'queue' ]
  try:
    for fn in runFnA:
      f      = open(fn,"r")
      runT   = json.loads(f.read())
      f.close()
      conn   = xalt.connect()
      query  = "USE "+xalt.db()
      conn.query(query)

      translate(nameA, runT['envT'], runT['userT']);
      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S",
                                  time.localtime(float(runT['userT']['start_time'])))
      uuid        = runT['xaltLinkT'].get('uuid')
      if (uuid):
        uuid = "'" + uuid + "'"
      else:
        uuid = "NULL"

      #print( "Looking for run_uuid: ",runT['userT']['run_uuid'])

      query = "SELECT run_id FROM xalt_job WHERE run_uuid='%s'" % runT['userT']['run_uuid']
      conn.query(query)

      result = conn.store_result()
      if (result.num_rows() > 0):
        #print("found")
        row    = result.fetch_row()
        run_id = int(row[0][0])
        query  = "UPDATE xalt_job SET run_time='%.2f', end_time='%.2f' WHERE run_id='%d'" % (
          runT['userT']['run_time'], runT['userT']['end_time'], run_id)
        conn.query(query)
        return
      else:
        #print("not found")
        query  = "INSERT INTO xalt_job VALUES (NULL,'%s','%s','%s', '%s',%s,'%s', '%s','%s','%.2f', '%.2f','%.2f','%d', '%d','%d','%s', '%s','%s','%s') " % (
          runT['userT']['job_id'],      runT['userT']['run_uuid'],    dateTimeStr,
          runT['userT']['syshost'],     uuid,                         runT['hash_id'],
          runT['userT']['account'],     runT['userT']['exec_type'],   runT['userT']['start_time'],
          runT['userT']['end_time'],    runT['userT']['run_time'],    runT['userT']['num_cores'],
          runT['userT']['num_nodes'],   runT['userT']['num_threads'], runT['userT']['queue'],
          runT['userT']['user'],        runT['userT']['exec_path'],   runT['userT']['cwd'])
        conn.query(query)
        run_id   = conn.insert_id()

      #print("run_id: ", run_id)

      # loop over shared libraries
      for entryA in runT['libA']:
        object_path = entryA[0]
        hash_id     = entryA[1]
        query       = "SELECT obj_id FROM xalt_object WHERE hash_id='%s' AND object_path='%s' AND syshost='%s'" % ( 
          hash_id, object_path, runT['userT']['syshost'])
        conn.query(query)
        result = conn.store_result()
        if (result.num_rows() > 0):
          row    = result.fetch_row()
          obj_id = int(row[0][0])
        else:
          obj_kind = obj_type(object_path)
          query    = "INSERT into xalt_object VALUES (NULL,'%s','%s','%s',NOW(),'%s') " % (
                      object_path, runT['userT']['syshost'], hash_id, obj_kind)
          conn.query(query)
          obj_id   = conn.insert_id()

        #print("obj_id: ", obj_id)
        # Now link libraries to xalt_job record.
        query = "INSERT into join_job_object VALUES (NULL,'%d','%d') " % (
          obj_id, run_id)
        conn.query(query)

      # loop over env.
      for key in runT['envT']:
        value = runT['envT'][key]
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

        query = "INSERT INTO join_job_env VALUES (NULL, '%d', '%d', '%s')" % (
          env_id, run_id, value)
        conn.query(query)
        


  except MySQLdb.Error, e:
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)
  



def main():

  args = CmdLineOptions().execute()

  xalt = XALTdb(ConfigFn)

  strA = capture(['wc', '-l', '/etc/passwd']).split(' ')
  num  = int (strA[0])
  pbar = ProgressBar(maxVal=num,ttyOnly=True)
  icnt = 0

  t1 = time.time()

  for user, hdir in passwd_generator():
    xaltDir = os.path.join(hdir,".xalt.d")
    if (os.path.isdir(xaltDir)):
      linkFnA = files_in_tree(xaltDir, "*/link.*.json")
      link_json_to_db(xalt, user, linkFnA)
      if (args.delete):
        remove_files(linkFnA)

      runFnA = files_in_tree(xaltDir, "*/run.*.json")
      run_json_to_db(xalt, user, runFnA)
      if (args.delete):
        remove_files(runFnA)
    icnt += 1
    pbar.update(icnt)

  pbar.fini()
  t2 = time.time()
  rt = t2 - t1
  if (args.timer):
    print("Time: ", time.strftime("%T", time.gmtime(rt)))

if ( __name__ == '__main__'): main()
