#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

from __future__             import print_function
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.append(os.path.abspath(os.path.join(dirNm, "../libexec")))
from xalt_parse_mpirun_args import find_exec
import os, re, sys

ignoreT = {
  'env'              : True,
  'time'             : True,
}

argT = {
  '-am'                       : 1,
  '--app'                     : 1,
  '-c'                        : 1,
  '-n'                        : 1,
  '-np'                       : 1,
  '-cf'                       : 1,  
  '--cartofile'               : 1,
  '-cpus-per-proc'            : 1,
  '--cpus-per-proc'           : 1,
  '-cpus-per-rank'            : 1,
  '--cpus-per-rank'           : 1,
  '-H'                        : 1,
  '-host'                     : 1,
  '--host'                    : 1,
  '-launch-agent'             : 1,
  '--launch-agent'            : 1,
  '-machinefile'              : 1,
  '--machinefile'             : 1,
  '-mca'                      : 2,
  '--mca'                     : 2,
  '-nperboard'                : 1,
  '--nperboard'               : 1,
  '-npernode'                 : 1,
  '--npernode'                : 1,
  '-npersocket'               : 1,
  '--npersocket'              : 1,
  '-num-boards'               : 1,
  '--num-boards'              : 1,
  '-num-cores'                : 1,
  '--num-cores'               : 1,
  '-num-socket'               : 1,
  '--num-socket'              : 1,
  '-ompi-server'              : 1,
  '--ompi-server'             : 1,
  '-output-filename'          : 1,
  '--output-filename'         : 1,
  '-path'                     : 1,
  '--path'                    : 1,
  '--prefix'                  : 1,
  '--preload-files'           : 1,     
  '--preload-files-dest-dir'  : 1,
  '-report-events'            : 1,     
  '--report-events'           : 1,     
  '-report-pid'               : 1,     
  '--report-pid'              : 1,     
  '-report-uri'               : 1,     
  '--report-uri'              : 1,     
  '-rf'                       : 1,     
  '--rankfile'                : 1,     
  '-server-wait-time'         : 1,
  '--server-wait-time'        : 1,
  '-slot-list'                : 1,
  '--slot-list'               : 1,
  '-stdin'                    : 1,
  '--stdin'                   : 1,
  '-stride'                   : 1,
  '--stride'                  : 1,
  '-tmpdir'                   : 1,
  '--tmpdir'                  : 1,
  '-wd'                       : 1,
  '--wd'                      : 1,
  '-wdir'                     : 1,
  '--wdir'                    : 1,
  '-xml'                      : 1,
  '--xml'                     : 1,
  '-xml-file'                 : 1,
  '--xml-file'                : 1,
  '-xterm'                    : 1,
  '--xterm'                   : 1,
}

def main():
  print(find_exec(ignoreT, argT, None, sys.argv[1:]))

if ( __name__ == '__main__'): main()
