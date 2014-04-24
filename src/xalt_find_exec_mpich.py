#!/usr/bin/env python
# -*- python -*-
from __future__             import print_function
from xalt_parse_mpirun_args import find_exec
import os, re, sys

ignoreT = {
  'env'              : True,
  'time'             : True,
}

argT = {
  '-genv'                     : 2,
  '-genvlist'                 : 1,
  '-f'                        : 1,
  '-n'                        : 1,  
  '-np'                       : 1,  
  '-hosts'                    : 1,
  '-configfile'               : 1,
  '-launcher'                 : 1,
  '-launcher-exec'            : 1,
  '-rmk'                      : 1,
}

def main():
  print(find_exec(ignoreT, argT, sys.argv[1:]))

if ( __name__ == '__main__'): main()
