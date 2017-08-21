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
#
# This routine needs to set a few special fields that go into the
# database.  We try to get the "account", "job_id", "num_cores",
# "num_nodes", "num_threads", "queue" and "submithost" from the job.
# Some of these are static for the job while some change for each
# use of the code launcher.  This file is just an example of how a
# couple sites get this information.  
#
# You see information like "account" and "job_id" can usually be
# obtained from the scheduler environment.
#
# Other information like num_cores you probably need to get from the
# the job launcher.  And this is where it gets tricky - below we 
# don't set num_cores for the PBS example because it is already set
# by the aprun and mpirun job launchers and passed through to the
# xalt_run_submission.py script. 
#
# num_threads is assumed to OMP_NUM_THREADS and obtained in 
# xalt_run_submission.py.
#
# Other information like may or may not be obtainable depending on 
# the job launcher and what not.  The degree to which you get this 
# information directly results in what kinds of reports you can do.  
# If you don't have an accurate accounting for the number of cores 
# used (for example), then any reports that yield core hour results 
# may not be useful.
#
from __future__ import print_function
import os, re


def translate(nameA, envT, userT, userDT):
  """
  Site Package prototype.
  """
  sysT = {}
  queueType = "SLURM"
  if (envT.get("SGE_ACCOUNT")):
    queueType = "SGE"
  elif (envT.get("SLURM_TACC_ACCOUNT") or envT.get("SLURM_TACC_JOBNAME")):
    queueType = "SLURM_TACC"
  elif (envT.get("SBATCH_ACCOUNT")):
    queueType = "SLURM"
  elif (envT.get("PBS_JOBID")):
    queueType = "PBS"
  elif (envT.get("LSF_VERSION")):
    queueType = "LSF"
      
  if (queueType == "SGE"):
    userDT['num_cores']    = "NSLOTS"
    sysT['num_nodes']      = "NHOSTS"
    sysT['account']        = "SGE_ACCOUNT"
    sysT['job_id']         = "JOB_ID"
    sysT['queue']          = "QUEUE"
      
  elif (queueType == "SLURM_TACC"):
    num_nodes              = int(envT.get("SLURM_NNODES",0))
    userDT['num_cores']    = userDT['num_tasks']
    sysT['num_nodes']      = "SLURM_NNODES"
    sysT['account']        = "SLURM_TACC_ACCOUNT"
    sysT['job_id']         = "SLURM_JOB_ID"
    sysT['queue']          = "SLURM_QUEUE"
    sysT['submit_host']    = "SLURM_SUBMIT_HOST"
  
  elif (queueType == "SLURM"):
    num_nodes              = int(envT.get("SLURM_NNODES",0))
    coresPerNode           = int(envT.get("SLURM_CPUS_ON_NODE",0))
    userDT['num_cores']    = num_nodes*coresPerNode
    sysT['num_nodes']      = "SLURM_JOB_NUM_NODES"   # or SLURM_NNODES
    sysT['job_id']         = "SLURM_JOB_ID"
    sysT['queue']          = "SLURM_QUEUE"
    sysT['submit_host']    = "SLURM_SUBMIT_HOST"

  elif (queueType == "PBS"):
    userDT['num_cores']    = userDT['num_tasks']
    sysT['num_nodes']      = "PBS_NUM_NODES"
    sysT['account']        = "PBS_ACCOUNT"
    sysT['job_id']         = "PBS_JOBID"
    sysT['queue']          = "PBS_QUEUE"
    sysT['submit_host']    = "PBS_O_HOST"
  
  elif (queueType == "LSF"):
    userDT['num_cores']    = "LSB_DJOB_NUMPROC"
    mcpu_hostA             = envT.get("LSB_MCPU_HOSTS","a 1").split()
    userDT['num_nodes']    = len(mcpu_hostsA)/2
    sysT['account']        = "%%_UNKNOWN_%%"
    sysT['job_id']         = "LSB_JOBID"
    sysT['queue']          = "LSB_QUEUE"
    sysT['submit_host']    = "LSB_EXEC_CLUSTER"
  
  for name in nameA:
    result = "unknown"
    key    = sysT.get(name)
    if (key):
      result = envT.get(key,"unknown")
    if (not (name in userT)):
      userT[name] = result
    
  keyA = [ 'num_cores', 'num_nodes' ]

  for key in keyA:
    value = userDT.get(key,"unknown")
    if ( value == "unknown" ):
      userDT[key] = 0
    else:
      userDT[key] = int(userDT[key])    
  
  if (userT['job_id'] == "unknown"):
    userT['job_id'] = envT.get('JOB_ID','unknown')

AcceptListA = [
    re.compile(r"^.*PATH.*"),
    re.compile(r"^I_MPI.*"),
    re.compile(r"^KMP.*"),
    re.compile(r"^LC_.*"),
    re.compile(r"^LD.*"),
    re.compile(r"^LOADEDMODULES$"),
    re.compile(r"^MIC_.*"),
    re.compile(r"^MKL.*"),
    re.compile(r"^MPICH_.*"),
    re.compile(r"^MV2_.*"),
    re.compile(r"^OFFLOAD.*"),
    re.compile(r"^OMP.*"),
    re.compile(r"^PYTHON.*"),
    re.compile(r"^R_*"),
    re.compile(r"^SHELL$"),
    re.compile(r"^TARG$"),
    re.compile(r"^_LMFILES_$")
  ]    

RejectListA = [
    re.compile(r"^DDTPATH$"),
    re.compile(r"^INFOPATH$"),
    re.compile(r"^LMOD*"),
    re.compile(r"^MAKE_INC_PATH$"),
    re.compile(r"^MANPATH$"),
    re.compile(r"^MIC_ENV_PREFIX$"),
    re.compile(r"^MIC_TACC.*_DIR$"),
    re.compile(r"^MIC_TACC.*_INC"),
    re.compile(r"^MIC_TACC.*_LIB$"),
    re.compile(r"^MKLROOT$"),
    re.compile(r"^MODULEPATH_ROOT$"),
    re.compile(r"^MPICH_HOME$"),
    re.compile(r"^PKG_CONFIG_PATH$"),
    re.compile(r"^__.*")
  ]

def keep_env_var(var):
  keep = False
  for pat in AcceptListA:
    m = pat.search(var)
    if (m):
      keep = True
      break
  if (not keep):
    return keep

  for pat in RejectListA:
    m = pat.search(var)
    if (m):
      keep = False
      return keep
  return keep
    
