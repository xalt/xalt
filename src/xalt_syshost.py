#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import socket

def nics_syshost(fqdn):
  nicsT = {
    "kraken"    : "kraken",
    "darter"    : "darter",
    "kid"       : "kids",
    "kfs"       : "keeneland",
    "titan"     : "titan",
    "nid00026"  : "mars",
    "nid00027"  : "mars",
    "verne"     : "verne",
    "athena"    : "anthena",
  }
  result = None
  for k in nicsT:
    if (k.find(fqdn,0) != -1):
      result = nicsT[k]
      break
  return result
  
def normal_syshost(fqdn):
  hostA = fqdn.split('.')
  idx = 1 
  if (len(hostA) < 2):
    idx = 0
  return hostA[idx]

def main():

  fqdn    = socket.getfqdn()
  syshost = nics_syshost(fqdn)
  if (not syshost):
    syshost = normal_syshost(fqdn)

  print(syshost)
  

if ( __name__ == '__main__'): main()
