#include "insert.h"
#include "translate.h"
#include <string.h>
#include <stdlib.h>
#include "xalt_obfuscate.h"

static const char * safe_get(const char* key, const char* defaultValue)
{
  const char* value = getenv(key);
  if (value)
    return value;
  return defaultValue;
}

typedef enum
{ 
 UNKNOWN = -1, SLURM = 1, PBS, SGE, LSF
} QueueType_t ;

QueueType_t queueType()
{
  const char* value;

  value = getenv("SLURM_JOB_ID");
  if (value)
    return SLURM;
    
  value = getenv("PBS_JOBID");
  if (value)
    return PBS;

  value = getenv("SGE_ACCOUNT");
  if (value)
    return SGE;

  value = getenv("LSF_VERSION");
  if (value)
    return LSF;

  return UNKNOWN;
}


void translate(S2S_t** userT, S2D_t** userDT)
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

  QueueType_t my_type = queueType();

  switch (my_type)
    {
    case SLURM:
      insert_key_string(userT,	 "scheduler",   "SLURM");
      insert_key_string(userT,	 "account",     safe_get( "SLURM_JOB_ACCOUNT",   "unknown"));
      insert_key_string(userT,	 "job_id",      safe_get( "SLURM_JOB_ID",        "unknown"));
      insert_key_string(userT,	 "queue",       safe_get( "SLURM_JOB_PARTITION", "unknown"));
      insert_key_string(userT,	 "submit_host", safe_get( "SLURM_SUBMIT_HOST",   "unknown"));

      num_nodes_str = safe_get( "SLURM_NNODES", "1");
      insert_key_double(userDT, "num_nodes",   strtod(num_nodes_str, (char **) NULL));
      break;
  
    case PBS:
      job_id_str = safe_get( "PBS_JOBID", "unknown");
      idx        = strspn(job_id_str, "][0123456789");
      utstring_new(    job_id2);
      utstring_bincpy( job_id2, job_id_str, idx);
	
      insert_key_string(userT,	 "scheduler",   "PBS");
      insert_key_string(userT,	 "account",     safe_get( "PBS_ACCOUNT", "unknown"));
      insert_key_string(userT,	 "job_id",      utstring_body(job_id2));
      insert_key_string(userT,	 "queue",       safe_get( "PBS_QUEUE",   "unknown"));
      insert_key_string(userT,	 "submit_host", safe_get( "PBS_O_HOST",  "unknown"));

      num_nodes_str = safe_get( "PBS_NUM_NODES", "1");
      insert_key_double(userDT, "num_nodes",   strtod(num_nodes_str, (char **) NULL));
      utstring_free(job_id2);
      break;
  
    case SGE:
      insert_key_string(userT, "scheduler",   "SGE");
      insert_key_string(userT, "account",     safe_get( "SGE_ACCOUNT", "unknown"));
      insert_key_string(userT, "job_id",      safe_get( "JOB_ID",      "unknown"));
      insert_key_string(userT, "queue",       safe_get( "QUEUE",       "unknown"));
      insert_key_string(userT, "submit_host", "unknown");

      num_nodes_str = safe_get( "HHOSTS", "1");
      insert_key_double(userDT, "num_nodes",   strtod(num_nodes_str, (char **) NULL));
      break;
  
    case LSF:
      {
	const char *p    = safe_get( "LSB_MCPU_HOSTS", "a 1");
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
      insert_key_string(userT,	"job_id",      safe_get( "LSF_JOBID",  	  "unknown"));
      insert_key_string(userT,	"queue",       safe_get( "LSF_QUEUE",  	  "unknown"));
      insert_key_string(userT,	"submit_host", safe_get( "LSF_EXEC_CLUSTER", "unknown"));
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
