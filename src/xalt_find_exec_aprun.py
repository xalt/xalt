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
dirNm, execName = os.path.split(sys.argv[0])
sys.path.insert(1,os.path.abspath(os.path.join(dirNm, "../libexec")))
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
  '-N'                        : 1,
  '--memory-per-pe'           : 1,
  '--pes'                     : 1,
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
  '-t'                        : 1,
  '--numa-nodes-per-node'     : 1,
  '--cpu-time-limit'          : 1,
}

npT = {
  '-n'                        : "tasks",
  '-d'                        : "threads",
  '--cpus-per-pe'             : "threads",
}  


def main():
  """
  Find name of executable when using aprun.
  """

  print(find_exec(ignoreT, argT, npT, None, sys.argv[1:], dot=True))

if ( __name__ == '__main__'): main()
