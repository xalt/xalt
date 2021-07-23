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
#
# Git Version: @git@

from __future__  import print_function
import os, sys, re, json, time
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.append(os.path.realpath(os.path.join(dirNm, "../site")))

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

class Rmap(object):
  """ This class files the reverseMap File from the reverse map directory"""
  def __init__(self, rmapD):
    """
    This ctor opens the reverse map directory. It looks for a file named
    "xalt_rmapT.json".  If that file is not found it looks for
    "xalt_rmapT.old.json".  The file is read in and converted to a
    python table.   The contents contain a timestampfn key value pair.
    
    The reverse map table is return in all cases except when the timestamp
    file exists and is newer than the reverseMap file.
    
    @param rmapD: The reverse map directory
    """
    self.__rmapT  = {}
    self.__libmap = {}
    if (not rmapD):
      return

    rmapA   = rmapD.split(':')
    searchA = [ ".json", ".old.json"]
    rmapFn  = None
    for dir in rmapA:
      for ext in searchA:
        rmapFn = os.path.join(dir, "xalt_rmapT" + ext)
        if (os.access(rmapFn, os.R_OK)):
          break
        rmapFn = None
      if (rmapFn):
        break

    if (rmapFn):
      if ( not os.path.isfile(rmapFn)):
        print("The path :\"",rmapFn,"\" is not a file -> aborting!")
        sys.exit(1)
        
      rmpMtime = os.stat(rmapFn).st_mtime
      f        = open(rmapFn,"r")
      t        = json.loads(f.read())
      f.close()
      self.__rmapT = t['reverseMapT']
      if 'xlibmap' in t:
        self.__libmap = t['xlibmap']
      return
        
    # Look for jsonReverseMapT.json and convert it to xalt_rmapT style.
    for dir in rmapA:
      for ext in searchA:
        rmapFn = os.path.join(dir, "jsonReverseMapT" + ext)
        if (os.access(rmapFn, os.R_OK)):
          break
        rmapFn = None
      if (rmapFn):
        break


    oldT = {}
    if (rmapFn):
      if (not os.path.isfile(rmapFn)):
        print("The path :\"",rmapFn,"\" is not a file -> aborting!")
        sys.exit(1)
      rmpMtime = os.stat(rmapFn).st_mtime
      f        = open(rmapFn,"r")
      t        = json.loads(f.read())
      f.close()
      oldT     = t['reverseMapT']
      if 'xlibmap' in t:
        self.__libmap = t['xlibmap']

    defaultPat = re.compile(r'default:?')
    rmapT = {}
    for key in oldT:
      entry = oldT[key]
      flavor = entry['flavor'][0]
      flavor = defaultPat.sub('',flavor)
      if (flavor):
        moduleName = entry['pkg'] + '(' + flavor + ')'
      else:
        moduleName = entry['pkg']
      rmapT[key] = moduleName

    self.__rmapT = rmapT

  def reverseMapT(self):
    """
    return the reverse map table found by the ctor.

    @return reverseMapT 
    """
    return self.__rmapT 

  def libMap(self):
    """
    return the library map table found by the ctor.

    @return reverseMapT 
    """
    return self.__libmap
