#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

from __future__             import print_function
import os, sys

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.abspath(os.path.join(dirNm, "../libexec")))

from xalt_parse_mpirun_args import find_exec

ignoreT = {
  'env'              : True,
  'time'             : True,
}

# these args work for SGI MPT 2.01 on an SGI UV1000
argT = {
  '-a'                        : 1,
  '-array'                    : 1,
  '-d'                        : 1,
  '-dir'                      : 1,
  '-f'                        : 1,
  '-file'                     : 1,
  '-p'                        : 1,
  '-prefix'                   : 1,
  '-np'                       : 1,
  '-up'                       : 1,
}

def main():
  print(find_exec(ignoreT, argT, None, sys.argv[1:]))

if ( __name__ == '__main__'): main()
