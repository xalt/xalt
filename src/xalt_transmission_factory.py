from __future__  import print_function
import os, sys, base64

class XALT_transmission_factory(object):
  def __init__(self):
    pass

  @staticmethod
  def build(name, syshost, fn):
    name = name.lower()
    if (name == "syslog"):
      obj = Syslog(syshost)
    else:                       # file
      obj = File(syshost, fn)
      
    return obj

class Syslog(XALT_transmission_factory):

  def __init__(self, syshost):
    self.__syshost = syshost

  def save(self, kind, s):
    pass


class File(XALT_transmission_factory):

  def __init__(self, syshost, fn):
    self.__syshost = syshost
    self.__fn      = fn

  def save(self, kind, s):
    dirname, fn = os.path.split(self.__fn)
    tmpFn       = os.path.join(dirname, "." + fn)
    if (not os.path.isdir(dirname)):
      os.mkdir(dirname);
    
    f = open(tmpFn,"w")
    f.write(s)
    f.write(s)
    f.close()
    os.rename(tmpFn, self.__fn)
