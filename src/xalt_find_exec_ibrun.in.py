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
  'tacc_affinity'    : True,
  'time'             : True,
}

argT = {
  '-2'                        : 1,
  '-m'                        : 1,
  '-n'                        : 1,  
  '-np'                       : 1,  
  '-o'                        : 1,  
}


def main():

  print(find_exec(ignoreT, argT, "-c", sys.argv[1:]))

if ( __name__ == '__main__'): main()
