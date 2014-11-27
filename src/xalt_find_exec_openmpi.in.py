# -*- python -*-
#
# Git Version: @git@

#-----------------------------------------------------------------------
# XALT: A tool that tracks users jobs and environments on a cluster.
# Copyright (C) 2013-2014 University of Texas at Austin
# Copyright (C) 2013-2014 University of Tennessee
# 
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of 
# the License, or (at your option) any later version. 
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser  General Public License for more details. 
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA
#-----------------------------------------------------------------------

from __future__             import print_function
import os, re, sys
dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.abspath(os.path.join(dirNm, "../libexec")))
from xalt_parse_mpirun_args import find_exec

ignoreT = {
  'env'              : True,
  'time'             : True,
}

argT = {
  '-am'                       : 1,
  '--app'                     : 1,
  '-c'                        : 1,
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

npT = {
  '-n'                        : "tasks",
  '-np'                       : "tasks",
}

def main():
  """
  Find name of executable when using openmpi
  """
  print(find_exec(ignoreT, argT, None, sys.argv[1:], dot=True))

if ( __name__ == '__main__'): main()
