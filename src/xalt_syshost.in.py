#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

from __future__ import print_function
import socket, platform

def nics_syshost(nameA):
  nicsT = {
    "kraken"    : "kraken",
    "darter"    : "darter",
    "nid00129"  : "darter",
    "nid00453"  : "darter",
    "kid"       : "kids",
    "kfs"       : "keeneland",
    "titan"     : "titan",
    "nid00026"  : "mars",
    "nid00027"  : "mars",
    "verne"     : "verne",
    "nautilus"  : "nautilus",
    "conseil"   : "nautilus",
  }
  result = None

  for name in nameA:
    for k in nicsT:
      if (name.find(k,0) != -1):
        result = nicsT[k]
        return result
  return result
  
def normal_syshost(nameA):
  
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
    syshost = normal_syshost(nameA)

  print(syshost)
  

if ( __name__ == '__main__'): main()
