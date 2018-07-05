#include "run_submission.h"
#include <stdio.h>
#include <string.h>

const char * safe_get(Table& t, const char* key, const char* defaultValue)
{
  Table::const_iterator got = t.find(key);
  if (got == t.end())
    return defaultValue;
  return got->second.c_str();
}

void translate(Table& envT, Table& userT, DTable& userDT)
{
  enum QueueType { UNKNOWN = -1, SLURM = 1, SGE, SLURM_TACC, PBS, LSF };
  QueueType queueType = UNKNOWN;

  // Pick type of queuing system.


  if (envT.count("SGE_ACCOUNT"))
    queueType = SGE;
  else if (envT.count("SLURM_TACC_ACCOUNT") || envT.count("SLURM_TACC_JOBNAME"))
    queueType = SLURM_TACC;
  else if (envT.count("SBATCH_ACCOUNT"))
    queueType = SLURM;
  else if (envT.count("PBS_JOBID"))
    queueType = PBS;
  else if (envT.count("LSF_VERSION"))
    queueType = LSF;

  // userDT["num_tasks"] has a safe default value of 1 if not overridden by the run.
  userDT["num_cores"]  = userDT["num_tasks"]; 

  // Now fill in num_cores, num_nodes, account, job_id, queue, submit_host in userT from the environment
  if (queueType == SGE)
    {
      userT["account"]     = safe_get(envT,        "SGE_ACCOUNT", "unknown");
      userT["job_id"]      = safe_get(envT,        "JOB_ID",      "unknown");
      userT["queue"]       = safe_get(envT,        "QUEUE",       "unknown");
      userT["submit_host"] = "unknown";
      userDT["num_nodes"]  = strtod(safe_get(envT, "NHOSTS",      "1"),        (char **) NULL);
    }
  else if (queueType == SLURM_TACC || queueType == SLURM )
    {
      userT["job_id"]      = safe_get(envT,        "SLURM_JOB_ID",        "unknown");
      userT["queue"]       = safe_get(envT,        "SLURM_QUEUE",         "unknown");
      userT["submit_host"] = safe_get(envT,        "SLURM_SUBMIT_HOST",   "unknown");
      userT["account"]     = safe_get(envT,        "SLURM_TACC_ACCOUNT",  "unknown");
      userDT["num_nodes"]  = strtod(safe_get(envT, "SLURM_NNODES",        "1"),       (char **) NULL);
    }
  else if (queueType == PBS)
    {
      std::string job_id   = safe_get(envT,        "PBS_JOBID",     "unknown");
      std::size_t idx      = job_id.find_first_not_of("0123456789");
      userT["job_id"]      = job_id.substr(0,idx);
      userT["queue"]       = safe_get(envT,        "PBS_QUEUE",     "unknown");
      userT["submit_host"] = safe_get(envT,        "PBS_O_HOST",    "unknown");
      userT["account"]     = safe_get(envT,        "PBS_ACCOUNT",   "unknown");
      userDT["num_nodes"]  = strtod(safe_get(envT, "PBS_NUM_NODES", "1"),       (char **) NULL);;
    }
  else if (queueType == LSF)
    {
      // We must count the number of "words" in mcpuA.
      // We find the number of words by counting space blocks and add 1;
      // then divide by 2. then convert to a string.
      std::string mcpuA    = safe_get(envT,  "LSB_MCPU_HOSTS",  "a 1");
      std::string::size_type idx;
      int count = 1;
      idx = 0;
      while (1)
        {
          idx = mcpuA.find(" ",idx);
          if (idx == std::string::npos)
            break;
          count++;
          idx = mcpuA.find_first_not_of(" ",idx+1);
        }
      count /= 2;

      userT["job_id"]      = safe_get(envT,  "LSB_JOBID",        "unknown");
      userT["queue"]       = safe_get(envT,  "LSB_QUEUE",        "unknown");
      userT["submit_host"] = safe_get(envT,  "LSB_EXEC_CLUSTER", "unknown");
      userT["account"]     = "unknown";
      userDT["num_nodes"]  = (double) count;
    }
  else
    {
      userT["job_id"]      = "unknown";
      userT["queue"]       = "unknown";
      userT["submit_host"] = "unknown";
      userT["account"]     = "unknown";
      userDT["num_nodes"]  = 1.0;
    }
}
