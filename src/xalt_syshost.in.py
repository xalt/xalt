#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@
#
# user defined function
# this is only an example that works at a couple sites
#

from __future__ import print_function
import socket, platform

# must set this to True or False depending on your naming scheme
# unless you plan to do a manual mapping of all your machine names
# see below for description of level1 and level2
level1format = False

def map_syshost(nameA):
  nicsT = {
    "kraken"       : "kraken",
    "aprun-darter" : "darter",
    "kid"          : "kids",
    "kfs"          : "keeneland",
    "titan"        : "titan",
    "nid00026"     : "mars",
    "nid00027"     : "mars",
    "nid00043"     : "mars",
    "nid00053"     : "mars",
    "verne"        : "verne",
    "nautilus"     : "nautilus",
    "conseil"      : "nautilus",
    "arronax"      : "nautilus",
    "harpoon"      : "nautilus",
  }
  result = None

  for name in nameA:
    for k in nicsT:
      if (name.find(k,0) != -1):
        result = nicsT[k]
        return result
  return result
  
def level1_syshost(nameA):
  # Returns the 1st level xx of the xx.yy.zz.ww hostname name.
  # Useful for naming conventions like
  #     machine1.dept.institute.edu
  # where you want to return the machine name without the number.

  # use the platform.node result provided in nameA array
  #   and strip off trailing numbers

  name = nameA[1].rstrip('1234567890')  
  return name

def level2_syshost(nameA):
  # Returns the 2nd level yy of the xx.yy.zz.ww hostname name.
  # Useful for naming conventions like
  #     login1.machine.site.edu
  # where you want to return the machine name.

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

  nameA = [ socket.getfqdn(),
            platform.node() ]

  syshost = nics_syshost(nameA)
  if (not syshost):
    if (level1format):
      syshost = level1_syshost(nameA)
    else:
      syshost = level2_syshost(nameA)

  print(syshost)
  

if ( __name__ == '__main__'): main()
