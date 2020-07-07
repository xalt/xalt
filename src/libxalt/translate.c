#include "insert.h"
#include "translate.h"
#include <string.h>

const char * safe_get(S2S_t* envT, const char* key, const char* defaultValue)
{
  S2S_t *entry;
  HASH_FIND_STR(envT, key, entry);
  if (entry == NULL) 
    return defaultValue;
  return utstring_body(entry->value);
}

typedef enum
{ 
 UNKNOWN = -1, SLURM = 1, PBS, SGE, LSF
} QueueType_t ;

QueueType_t queueType(S2S_t* envT)
{
  S2S_t *entry;

  HASH_FIND_STR(envT, "SLURM_JOB_ID", entry);
  if (entry)
    return SLURM;

  HASH_FIND_STR(envT, "PBS_JOBID", entry);
  if (entry)
    return PBS;

  HASH_FIND_STR(envT, "SGE_ACCOUNT", entry);
  if (entry)
    return SGE;

  HASH_FIND_STR(envT, "LSF_VERSION", entry);
  if (entry)
    return LSF;

  return UNKNOWN;
}


void translate(S2S_t* envT, S2S_t** userT, S2D_t** userDT)
{
  size_t    idx;
  UT_string *job_id2;
  const char* num_nodes_str;
  const char* job_id_str;
  double num_cores = 1.0;
  double num_nodes = 1.0;
  S2D_t* entry;

  HASH_FIND_STR(*userDT, "num_tasks", entry);
  if (entry)
    num_cores = entry->value;
  insert_key_double(userDT, "num_cores", num_cores);

  QueueType_t my_type = queueType(envT);

  switch (my_type)
    {
    case SLURM:
      insert_key_string(userT,	 "scheduler",   "SLURM");
      insert_key_string(userT,	 "account",     safe_get(envT, "SLURM_JOB_ACCOUNT",   "unknown"));
      insert_key_string(userT,	 "job_id",      safe_get(envT, "SLURM_JOB_ID",        "unknown"));
      insert_key_string(userT,	 "queue",       safe_get(envT, "SLURM_JOB_PARTITION", "unknown"));
      insert_key_string(userT,	 "submit_host", safe_get(envT, "SLURM_SUBMIT_HOST",   "unknown"));

      num_nodes_str = safe_get(envT, "SLURM_NNODES", "1");
      insert_key_double(userDT, "num_nodes",   strtod(num_nodes_str, (char **) NULL));
      break;
  
    case PBS:
      job_id_str = safe_get(envT, "PBS_JOBID", "unknown");
      idx        = strspn(job_id_str, "][0123456789");
      utstring_new(    job_id2);
      utstring_bincpy( job_id2, job_id_str, idx);
	
      insert_key_string(userT,	 "scheduler",   "PBS");
      insert_key_string(userT,	 "account",     safe_get(envT, "PBS_ACCOUNT", "unknown"));
      insert_key_string(userT,	 "job_id",      utstring_body(job_id2));
      insert_key_string(userT,	 "queue",       safe_get(envT, "PBS_QUEUE",   "unknown"));
      insert_key_string(userT,	 "submit_host", safe_get(envT, "PBS_O_HOST",  "unknown"));

      num_nodes_str = safe_get(envT, "PBS_NUM_NODES", "1");
      insert_key_double(userDT, "num_nodes",   strtod(num_nodes_str, (char **) NULL));
      utstring_free(job_id2);
      break;
  
    case SGE:
      insert_key_string(userT, "scheduler",   "SGE");
      insert_key_string(userT, "account",     safe_get(envT, "SGE_ACCOUNT", "unknown"));
      insert_key_string(userT, "job_id",      safe_get(envT, "JOB_ID",      "unknown"));
      insert_key_string(userT, "queue",       safe_get(envT, "QUEUE",       "unknown"));
      insert_key_string(userT, "submit_host", "unknown");

      num_nodes_str = safe_get(envT, "HHOSTS", "1");
      insert_key_double(userDT, "num_nodes",   strtod(num_nodes_str, (char **) NULL));
      break;
  
    case LSF:
      {
	const char *p    = safe_get(envT, "LSB_MCPU_HOSTS", "a 1");
	int        count = 1;
	while(1)
	  {
	    p = strchr(p,' ');
	    if (p == NULL)
	      break;
	    count++;
	    p += strspn(p," ");
	  }
	num_nodes = (double) (count/2);
      }
      insert_key_string(userT,	"scheduler",   "LSF");
      insert_key_string(userT,	"account",     "unknown");
      insert_key_string(userT,	"job_id",      safe_get(envT, "LSF_JOBID",  	  "unknown"));
      insert_key_string(userT,	"queue",       safe_get(envT, "LSF_QUEUE",  	  "unknown"));
      insert_key_string(userT,	"submit_host", safe_get(envT, "LSF_EXEC_CLUSTER", "unknown"));
      insert_key_double(userDT, "num_nodes",   num_nodes);
      break;

    default:
      insert_key_string(userT,	"scheduler",   "unknown");
      insert_key_string(userT,	"account",     "unknown");
      insert_key_string(userT,	"job_id",      "unknown");
      insert_key_string(userT,	"queue",       "unknown");
      insert_key_string(userT,	"submit_host", "unknown");
      insert_key_double(userDT, "num_nodes",   1.0);
      break;
    }
}
