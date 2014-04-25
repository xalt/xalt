from __future__ import print_function
import os

def tranlate(nameA, envT, userT):
  sysT = {}
  queueType = "SLURM"
  if (envT.get("SGE_ACCOUNT")):
    queueType = "SGE"
  elif (envT.get("SLURM_TACC_ACCOUNT")):
    queueType = "SLURM"
      
  if (queueType == "SGE"):
    sysT['num_cores'] = "NSLOTS"
    sysT['num_nodes'] = "NHOSTS"
    sysT['account']   = "SGE_ACCOUNT"
    sysT['job_id']    = "JOB_ID"
    sysT['queue']     = "QUEUE"
      
  elif (queueType == "SLURM"):
    sysT['num_cores'] = "SLURM_TACC_CORES"
    sysT['num_nodes'] = "SLURM_NNODES"
    sysT['account']   = "SLURM_TACC_ACCOUNT"
    sysT['job_id']    = "SLURM_JOB_ID"
    sysT['queue']     = "SLURM_QUEUE"
  
  for name in nameA:
    result = "unknown"
    key    = sysT.get(name)
    if (key):
      result = envT.get(key,"unknown")
    userT[name] = result
    
