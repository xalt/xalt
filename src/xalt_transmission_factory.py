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
from __future__  import print_function
import os, sys, json, base64

try:
  from XALTdb      import XALTdb
  XALTdb_available = True
except ImportError:
  XALTdb_available = False
  
  
from XALT_Rmap   import Rmap
from xalt_global import *

class XALT_transmission_factory(object):
  """
  This class is a factory to determine which way to "write" the json data.
  """
  def __init__(self, syshost, kind):
    """
    This ctor saves away the system name and kind of record (link, run)

    @param syshost: Name of the system.
    @param kind:  Type of record: link or run
    """

    self.__syshost = syshost
    self.__kind    = kind

  def _syshost(self):
    """
    returns the system name
    @return returns the system name
    """
    return self.__syshost

  def _kind(self):
    """
    Returns the kind of record: link or run
    @return the kind of record.
    """
    return self.__kind

  @staticmethod
  def build(name, syshost, kind, fn):
    """
    The static factory build routine that returns the transmission object.

    @param name: Name of the factory: syslog, directdb, file
    @param syshost: The system name.  Should be stampede or darter not login1.stampede....
    @param kind: Type of record: link, run
    @param fn:  file name (only used by file transport object
    """

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
  """
  This class write the json record to syslog
  """

  def __init__(self, syshost, kind):
    """
    This is the ctor for Syslog transmission method
    @param syshost: Name of the system.
    @param kind:  Type of record: link or run
    """

    super(Syslog, self).__init__(syshost, kind)
  def save(self, resultT):
    """
    The json table is written to syslog with the text converted to base64.
    @param resultT: The json record table
    """
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
  """
  This is the file transport class
  """

  def __init__(self, syshost, kind, fn):
    """
    This ctor is for the file transport method.
    @param syshost: Name of the system.
    @param kind:  Type of record: link or run
    @param fn:  the file name to write the record to.
    """
    super(File, self).__init__(syshost, kind)
    self.__fn      = fn

  def save(self, resultT):
    """
    The json table is written to the file specified in the ctor.
    @param resultT: The json record table
    """
    s           = json.dumps(resultT, sort_keys=True,
                             indent=2, separators=(',',': '))
    dirname, fn = os.path.split(self.__fn)
    tmpFn       = os.path.join(dirname, "." + fn)
    
    try:
      if (not os.path.isdir(dirname)):
        os.mkdir(dirname);
    
      f = open(tmpFn,"w")
      f.write(s)
      f.close()
      os.rename(tmpFn, self.__fn)
    except:
      pass


class DirectDB(XALT_transmission_factory):
  """
  This class is the direct to db transmission method.
  """

  def __init__(self, syshost, kind):
    """
    This is the ctor for Direct to DB transmission method
    @param syshost: Name of the system.
    @param kind:  Type of record: link or run
    """
    super(DirectDB, self).__init__(syshost, kind)
  def save(self, resultT):
    """
    The json table is written directly to the db.
    @param resultT: The json record table
    """
    if (not XALTdb_available):
      raise ImportError
    
    ConfigFn     = os.path.join(XALT_ETC_DIR,"xalt_db.conf")
    RMapFn       = os.path.join(XALT_ETC_DIR,"reverseMapD")

    xalt        = XALTdb(ConfigFn)
    reverseMapT = Rmap(RMapFn).reverseMapT()
    if (self._kind() == "link"):
      xalt.link_to_db(reverseMapT, resultT)
    else: 
      # kind == "run"
      xalt.run_to_db(reverseMapT, resultT)
