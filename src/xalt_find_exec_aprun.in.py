#!/usr/bin/env python
# -*- python -*-
from __future__             import print_function
import os, re, sys
dirNm, execName = os.path.split(sys.argv[0])
sys.path.append(os.path.abspath(os.path.join(dirNm, "../libexec")))
from xalt_parse_mpirun_args import find_exec

ignoreT = {
  'env'              : True,
  'time'             : True,
}

# option and number of arguments space delimited
argT = {
  '-a'                        : 1,
  '--architecture'            : 1,
  '-cc'                       : 1,
  '--cpu-binding'             : 1,
  '-cp'                       : 1,
  '--cpu-binding-file'        : 1,
  '-d'                        : 1,
  '--cpus-per-pe'             : 1,
  '-D'                        : 1,
  '--debug'                   : 1,
  '-e'                        : 1,
  '-E'                        : 1,
  '--exclude-node-list'       : 1,
  '--exclude-node-list-file'  : 1,
  '-F'                        : 1,
  '--access-mode'             : 1,
  '-j'                        : 1,
  '--CPUs'                    : 1,
  '-L'                        : 1,
  '-l'                        : 1,
  '-m'                        : 1,
  '--memory-per-pe'           : 1,
  '-n'                        : 1,
  '--pes'                     : 1,
  '-N'                        : 1,
  '--pes-per-node'            : 1,
  '-p'                        : 1,
  '--p-governor'              : 1,
  '--p-state'                 : 1,
  '-r'                        : 1,
  '--specialized-cpus'        : 1,
  '-R'                        : 1,
  '--relaunch'                : 1,
  '-S'                        : 1,
  '--pes-per-numa-node'       : 1,
  '-sl'                       : 1,
  '--numa-node-list'          : 1,
  '-sn'                       : 1,
  '--numa-nodes-per-node'     : 1,
  '-t'                        : 1,
  '--cpu-time-limit'          : 1,
}

def main():
  print(find_exec(ignoreT, argT, None, sys.argv[1:]))

if ( __name__ == '__main__'): main()
