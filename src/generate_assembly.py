#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, socket

def main():
  uuid = sys.argv[1]
  fn   = sys.argv[2]
  name = socket.getfqdn().lower()
  
  print("name: ",name)

  



if ( __name__ == '__main__'): main()
