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
import socket, platform

# must set this to True or False depending on your naming scheme
# unless you plan to do a manual mapping of all your machine names
# see below for description of level1 and level2
level1format = False

hostPatt = re.compile(r'nid[0-9][0-9][0-9][0-9][0-9]$')

def map_syshost(nameA):
  """
  Use the nameA array to report a system name
  """

  for name in nameA:
    m = hostPatt.match(name)
    if (m):
      return 'ls5'

  return None
  
def level2_syshost(nameA):
  """
  Returns the 2nd level yy of the xx.yy.zz.ww hostname name.
  Useful for naming conventions like
      login1.machine.site.edu
  where you want to return  "machine" as the name.

  """

  maxN = 0
  j    = -1
  i    = -1
  for name in nameA:
    i     = i + 1
    hostA = name.split('.')
    num   = len(hostA)
    if (num > maxN):
      j  = i
      maxN = num
  hostA = nameA[j].split('.')

  idx = 1 
  if (len(hostA) < 2):
    idx = 0
  return hostA[idx]

def main():
  """
  This command tries to report the system name base on host name information.
  It should return darter and not login1.darter.
  """

  nameA = [ socket.getfqdn(),
            platform.node() ]

  syshost = map_syshost(nameA)
  if (not syshost):
    syshost = level2_syshost(nameA)

  print(syshost)
  

if ( __name__ == '__main__'): main()
