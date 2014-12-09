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
import os, sys

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.abspath(os.path.join(dirNm, "../libexec")))

from xalt_parse_mpirun_args import find_exec

ignoreT = {
  'env'              : True,
  'time'             : True,
}

argT = {
  '-arch'                     : 1,
  '-bind-to'                  : 1,
  '-binding'                  : 1,
  '-bootstrap'                : 1,
  '-bootstrap-exec'           : 1,
  '-branch-count'             : 1,
  '-ckpoint-interval'         : 1,
  '-ckpoint-lib'              : 1,
  '-ckpoint-logfile'          : 1,
  '-ckpoint-num'              : 1,
  '-ckpoint-prefix'           : 1,
  '-config'                   : 1,
  '-configfile'               : 1,
  '-demux'                    : 1,
  '-env'                      : 2,
  '-envlist'                  : 1,
  '-f'                        : 1,
  '-file'                     : 1,
  '-genv'                     : 2,
  '-genvlist'                 : 1,
  '-gpath'                    : 1,
  '-gumask'                   : 1,
  '-gwdir'                    : 1,
  '-host'                     : 1,
  '-hostfile'                 : 1,
  '-hosts'                    : 1,
  '-iface'                    : 1,
  '-launcher'                 : 1,
  '-launcher-exec'            : 1,
  '-localhost'                : 1,
  '-machine'                  : 1,
  '-machinefile'              : 1,
  '-map-to'                   : 1,
  '-maxtime'                  : 1,
  '-membind'                  : 1,
  '-nameserver'               : 1,
  '-order-nodes'              : 1,
  '-path'                     : 1,
  '-perhost'                  : 1,
  '-pmi-connect'              : 1,
  '-s'                        : 1,
  '-sg'                       : 1,
  '-grr'                      : 1,
  '-ppn'                      : 1,
  '-rmk'                      : 1,
  '-soft'                     : 1,
  '-tmpdir'                   : 1,
  '-topolib'                  : 1,
  '-tva'                      : 1,
  '-idba'                     : 1,
  '-gdba'                     : 1,
  '-umask'                    : 1,
  '-usize'                    : 1,
  '-wdir'                     : 1,
}

npT = {
  '-n'                        : "tasks",  
  '-np'                       : "tasks",  
}  


def main():
  """
  Find name of executable when using mpich.
  """
  print(find_exec(ignoreT, argT, npT, None, sys.argv[1:]))

if ( __name__ == '__main__'): main()
