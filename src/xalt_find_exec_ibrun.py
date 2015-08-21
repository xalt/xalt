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
  'perf'             : True,
  'stat'             : True,
  'numactl'          : True,
  'tacc_affinity'    : True,
  'tacc_xrun'        : True,
  'time'             : True,
}

argT = {
  '-2'                        : 1,
  '-m'                        : 1,
  '-o'                        : 1,  
  '-c'                        : 1,  
}

npT = {
  '-n'                        : "tasks",  
  '-np'                       : "tasks",  
  }
def mpi_size(): 
  size = os.environ.get("PMI_SIZE",0)             + \  # MPICH and IMPI
         os.environ.get("OMPI_COMM_WORLD_SIZE",0) + \  # OpenMPI
         os.environ.get("MV2_COMM_WORLD_SIZE",0)       # Mvapich2
  return size
         
def mpi_rank(): 
  """
  Do we want ibrun_o_option part of rank or not?
  """

  rank = os.environ.get("PMI_RANK",0)             + \  # MPICH and IMPI
         os.environ.get("OMPI_COMM_WORLD_RANK",0) + \  # OpenMPI
         os.environ.get("MV2_COMM_WORLD_RANK",0)  + \  # Mvapich2
         os.environ.get("ibrun_o_option",0)            # IBRUN offset
  return rank


def compute_ntasks(t): 
  tasks = t.get("tasks",os.environ.get("SLURM_TACC_CORES",mpi_size())
  nodes = t.get("nodes",os.environ.get("SLURM_TACC_NODES"))
  if (not tasks):
    if (not nodes):
      tasks = 1
    elif (not os.environ.get("SLURM_JOBID")):
      tasks = 1
    if (nodes == 1):
      tasks = 1
    else:
      nodes = nodes or 1
      tasks = os.environ.get("SLURM_CPUS_ON_NODE",1)*nodes
  return tasks

def main():
  """
  Find name of executable when using ibrun.
  """

  print(find_exec(ignoreT, argT, npT, "-c", sys.argv[1:], compute_ntasks=compute_ntasks))

if ( __name__ == '__main__'): main()
