#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, socket

def name_selector(name):
  nameA=name.split(".")
  print(nameA[0])
  print(nameA[1])
  return nameA[1]



def main():
  uuid = sys.argv[1]
  fn   = sys.argv[2]
  name = name_selector(socket.getfqdn())
  
  print("name: ",name)

  



if ( __name__ == '__main__'): main()
