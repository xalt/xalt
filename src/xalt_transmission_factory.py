from __future__  import print_function
import os, sys, base64

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
    else:                 
      # file
      obj = File(syshost, kind, fn)
      
    return obj

class Syslog(XALT_transmission_factory):

  def __init__(self, syshost, kind):
    super(Syslog, self).__init__(syshost, kind)
  def save(self, s):
    pass


class File(XALT_transmission_factory):

  def __init__(self, syshost, kind, fn):
    super(File, self).__init__(syshost, kind)
    self.__fn      = fn

  def save(self, s):
    dirname, fn = os.path.split(self.__fn)
    tmpFn       = os.path.join(dirname, "." + fn)
    if (not os.path.isdir(dirname)):
      os.mkdir(dirname);
    
    f = open(tmpFn,"w")
    f.write(s)
    f.close()
    os.rename(tmpFn, self.__fn)
