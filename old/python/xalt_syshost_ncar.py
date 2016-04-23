# -*- python -*-
#
# Git Version: @git@
#
# user defined function
# this is only an example that works at a couple sites
#

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

#Si changed this on Mar 5, 2015
#Need the re module 
import re, platform

def map_syshost(name):
  """
  Use the name to report a system name
  """

  ncarT = {
    re.compile(r"^ys.*"): "yellowstone",
    re.compile(r"^geyser.*"): "geyser",
    re.compile(r"^caldera.*"):"caldera",
  }
  result = None

  for k in ncarT:
    m = k.search(name)
    if (m):
      result = ncarT[k]
      break
  return result
  
def main():
  """
  This command tries to report the system name base on host name information.
  It should return darter and not login1.darter.
  """

##  nameA = [ socket.getfqdn(),
##            platform.node() ]

  name = platform.node()

  syshost = map_syshost(name)
  if (not syshost):
    syshost = "unknown" 

  print(syshost)
  

if ( __name__ == '__main__'): main()
