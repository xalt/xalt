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
  '-genv'                     : 2,
  '-genvlist'                 : 1,
  '-f'                        : 1,
  '-hosts'                    : 1,
  '-configfile'               : 1,
  '-launcher'                 : 1,
  '-launcher-exec'            : 1,
  '-rmk'                      : 1,
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
