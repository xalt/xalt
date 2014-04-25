#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
from XALTdb     import XALTdb
from fnmatch    import fnmatch
import os, sys, re, MySQLdb, json, time
import warnings
warnings.filterwarnings("ignore", "Unknown table.*")

ConfigBaseNm = "xalt_db"
ConfigFn     = ConfigBaseNm + ".conf"


def files_in_tree(path, pattern):
  fileA = []
  wd = os.getcwd()
  if (not os.path.isdir(path)):
    return fileA

  os.chdir(path)
  path = os.getcwd()
  os.chdir(wd)

  for root, dirs, files in os.walk(path):
    for name in files:
      fn = os.path.join(root, name)
      if (fnmatch(fn,pattern)):
        fileA.append(fn)
  return fileA  

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

      dateTimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(float(linkT['build_epoch'])))
      # It is unique: lets store this link record
      query = "INSERT into xalt_link VALUES (NULL,'%s','%s','%s','%s','%s','%s','%.2f','%d','%s') " % (
               linkT['uuid'], linkT['hash_id'], dateTimeStr, linkT['link_program'], 
               linkT['build_user'], linkT['build_host'], float(linkT['build_epoch']),
               int(linkT['exit_code']), linkT['exec_path'])
      conn.query(query)
      link_id = conn.insert_id()
      print("link_id: ",link_id)

      for entryA in linkT['linkA']:
        object_path = entryA[0]
        hash_id     = entryA[1]

        query = "SELECT obj_id FROM xalt_object WHERE hash_id='%s' AND object_path='%s' AND build_host='%s'"
        conn.query(query)
        result = conn.store_result()
        if (result.num_rows() > 0):
          row    = result.fetch_row()
          obj_id = int(row[0][0])
          print("found old obj_id: ",obj_id)
        else:
          obj_kind = obj_type(object_path)

          query    = "INSERT into xalt_object VALUES (NULL,'%s','%s','%s',NOW(),'%s') " % (
                      object_path, linkT['build_host'], hash_id, obj_kind)
          conn.query(query)
          obj_id   = conn.insert_id()
          print("obj_id: ",obj_id, ", obj_kind: ", obj_kind,", path: ", object_path)

        # Now link libraries to xalt_link record:
        query = "INSERT into join_link_object VALUES (NULL,'%d','%d') " % (obj_id, link_id)
        conn.query(query)


  except MySQLdb.Error, e:
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit (1)
    
  

def main():
  xalt = XALTdb(ConfigFn)

  for user, hdir in passwd_generator():
    xaltDir = os.path.join(hdir,".xalt.d")
    if (os.path.isdir(xaltDir)):
      linkFnA = files_in_tree(xaltDir, "*/link.*.json")
      link_json_to_db(xalt, user, linkFnA)
      jobFnA = files_in_tree(xaltDir, "*/job.*.json")
      job_json_to_db(xalt, user, jobFnA)
      
      

if ( __name__ == '__main__'): main()
