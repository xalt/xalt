#include "run_submission.h"
#include <sstream>
const char * safe_get(Table& t, const char* key, const char* defaultValue)
{
  Table::const_iterator got = t.find(key);
  if (got == t.end())
    return defaultValue;
  return got->second.c_str();
}

void translate(Table& envT, Table& userT)
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


  // Now fill in num_cores, num_nodes, account, job_id, queue, submit_host in userT from the environment
  if (queueType == SGE)
    {
      userT["num_cores"]   = safe_get(userT, "num_tasks",   "1");
      userT["num_nodes"]   = safe_get(envT,  "NHOSTS",      "1");
      userT["account"]     = safe_get(envT,  "SGE_ACCOUNT", "unknown");
      userT["job_id"]      = safe_get(envT,  "JOB_ID",      "unknown");
      userT["queue"]       = safe_get(envT,  "QUEUE",       "unknown");
      userT["submit_host"] = "unknown";
    }
  else if (queueType == SLURM_TACC || queueType == SLURM )
    {
      userT["num_cores"]   = safe_get(userT, "num_tasks",           "1");
      userT["num_nodes"]   = safe_get(envT,  "SLURM_NNODES",        "1");
      userT["job_id"]      = safe_get(envT,  "SLURM_JOB_ID",        "unknown");
      userT["queue"]       = safe_get(envT,  "SLURM_QUEUE",         "unknown");
      userT["submit_host"] = safe_get(envT,  "SLURM_SUBMIT_HOST",   "unknown");
      if (queueType == SLURM_TACC)
        userT["account"]   = safe_get(envT,  "SLURM_TACC_ACCOUNT",  "unknown");
    }
  else if (queueType == PBS)
    {
      userT["num_cores"]   = safe_get(userT, "num_tasks",     "1");
      userT["num_nodes"]   = safe_get(envT,  "PBS_NUM_NODES", "1");
      userT["job_id"]      = safe_get(envT,  "PBS_JOBID",     "unknown");
      userT["queue"]       = safe_get(envT,  "PBS_QUEUE",     "unknown");
      userT["submit_host"] = safe_get(envT,  "PBS_O_HOST",    "unknown");
      userT["account"]     = safe_get(envT,  "PBS_ACCOUNT",   "unknown");
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
      std::ostringstream sstream;
      sstream << count;

      userT["num_cores"]   = safe_get(userT, "num_tasks",        "1");
      userT["num_nodes"]   = sstream.str();
      userT["job_id"]      = safe_get(envT,  "LSB_JOBID",        "unknown");
      userT["queue"]       = safe_get(envT,  "LSB_QUEUE",        "unknown");
      userT["submit_host"] = safe_get(envT,  "LSB_EXEC_CLUSTER", "unknown");
      userT["account"]     = "unknown";
    }
  else
    {
      userT["num_cores"]   = "1";
      userT["num_nodes"]   = "1";
      userT["job_id"]      = "unknown";
      userT["queue"]       = "unknown";
      userT["submit_host"] = "unknown";
      userT["account"]     = "unknown";
    }
}
