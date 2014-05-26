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
  print(find_exec(ignoreT, argT, None, sys.argv[1:]))

if ( __name__ == '__main__'): main()
