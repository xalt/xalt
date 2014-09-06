from __future__  import print_function
import os, sys, json, base64

from XALTdb      import XALTdb
from XALT_Rmap   import Rmap
from xalt_stack  import Stack

class XALT_transmission_factory(object):
  def __init__(self, syshost, kind):
    self.__syshost = syshost
    self.__kind    = kind

  def _syshost(self):
    return self.__syshost

  def _kind(self):
    return self.__kind

  @staticmethod
  def build(name, syshost, kind, fn):
    name = name.lower()
    if (name == "syslog"):
      obj = Syslog(syshost, kind)
    elif (name == "directdb"):
      obj = DirectDB(syshost, kind)
    else:                 
      # file
      obj = File(syshost, kind, fn)
      
    return obj

class Syslog(XALT_transmission_factory):

  def __init__(self, syshost, kind):
    super(Syslog, self).__init__(syshost, kind)
  def save(self, resultT):
    sA = []
    sA.append("logger -t XALT_LOGGING")
    sA.append(" \"")
    sA.append(self._kind())
    sA.append(":")
    sA.append(self._syshost())
    sA.append(":")
    sA.append(base64.b64encode(json.dumps(resultT)))
    sA.append("\"")
    s = "".join(sA)
    os.system(s)
    

class File(XALT_transmission_factory):

  def __init__(self, syshost, kind, fn):
    super(File, self).__init__(syshost, kind)
    self.__fn      = fn
  def save(self, resultT):
    s           = json.dumps(resultT, sort_keys=True,
                             indent=2, separators=(',',': '))
    dirname, fn = os.path.split(self.__fn)
    tmpFn       = os.path.join(dirname, "." + fn)
    if (not os.path.isdir(dirname)):
      os.mkdir(dirname);
    
    f = open(tmpFn,"w")
    f.write(s)
    f.close()
    os.rename(tmpFn, self.__fn)


class DirectDB(XALT_transmission_factory):

  def __init__(self, syshost, kind):
    super(DirectDB, self).__init__(syshost, kind)
  def save(self, resultT):
    # how do we get the xalt database and reverseMapT here?
    # hope they can be args passed
    # maybe paths are set at install time, and filename is hardcoded?
    # hardcoding it for now
    ConfigFn     = "/sw/tools/xalt/build/etc/xalt_db.conf"
    RMF          = "/sw/tools/xalt/build/etc/reverseMapD"

    xalt        = XALTdb(ConfigFn)
    reverseMapT = Rmap(RMF).reverseMapT()
    pstack      = Stack()
    if (self._kind() == "link"):
      xalt.link_to_db(pstack, reverseMapT, resultT)
    else: 
      # kind == "run"
      xalt.run_to_db(pstack, reverseMapT, resultT)

