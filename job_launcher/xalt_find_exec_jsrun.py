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
  '-n'                    : 1,
  '--nrs'                 : 1,
  '-c'                    : 1,
  '--cpu_per_rs'          : 1,
  '-g'                    : 1,
  '--gpu_per_rs'          : 1,
  '-m'                    : 1,
  '--memory_per_rs'       : 1,
  '-r'                    : 1,
  '--rs_per_host'         : 1,
  '-l'                    : 1,
  '--latency_priority'    : 1,
  '-S'                    : 1,
  '--save_resources'      : 1,
  '-U'                    : 1,
  '--use_resource'        : 1,
  '-a'                    : 1,
  '--tasks_per_rs'        : 1,
  '-f'                    : 1,
  '--appfile'             : 1,
  '-d'                    : 1,
  '--launch_distribution' : 1,
  '-t'                    : 1,
  '--stdio_input'         : 1,
  '-o'                    : 1,
  '--stdio_stdout'        : 1,
  '-e'                    : 1,
  '--stdio_mode'          : 1,
  '-k'                    : 1,
  '--stdio_stderr'        : 1,
  '-E'                    : 1,
  '--env'                 : 1,
  '-F'                    : 1,
  '--env_eval'            : 1,
  '-D'                    : 1,
  '--env_no_propagate'    : 1,
  '-L'                    : 1,
  '--use_spindle'         : 1,
  '-M'                    : 1,
  '--smpiargs'            : 1,
  '-P'                    : 1,
  '--pre_post_exec'       : 1,
  '-X'                    : 1,
  '--exit_on_error'       : 1,
  '-b'                    : 1,
  '--bind'                : 1,
  '-h'                    : 1,
  '--chdir'               : 1,
  '-u'                    : 1,
  '--debug'               : 1,
  '-i'                    : 1,
  '--immediate'           : 1,
}

npT = {
  '-p'   : "tasks",
  '--np' : "tasks",
}  


def main():
  """
  Find name of executable when using aprun.
  """
  try:
    print(find_exec(ignoreT, argT, npT, None, sys.argv[1:], dot=True))
  except Exception as e:
    print('find_exec_exception')
  
if ( __name__ == '__main__'): main()
