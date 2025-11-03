#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "buildUserT.h"
#include "buildEnvT.h"
#include "insert.h"
#include "translate.h"
#include <limits.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#define  DATESZ 100

int is_directory(const char *path)
{
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

void buildUserT(S2S_t** userT, S2D_t** userDT)
{
  S2D_t* d;
  S2S_t* e;
  char   dateStr[DATESZ];
  char   path[PATH_MAX+1];

  // CWD
  getcwd(path,sizeof(path));
  insert_key_string(userT, "cwd", path);

  // Epoch
  struct timeval tm;
  gettimeofday(&tm, NULL);
  double utc = tm.tv_sec + tm.tv_usec*1.e-6;

  UT_string* epoch_str;
  utstring_new(epoch_str);
  utstring_printf(epoch_str, "%f", utc);
  insert_key_string(userT, "currentEpoch", utstring_body(epoch_str));
  utstring_free(epoch_str);

  // start_date
  time_t mtime;
  double start_time = 0.0;
  HASH_FIND_STR(*userDT,"start_time", d);
  if (d != NULL)
    start_time = d->value;
  mtime = (time_t) start_time;
  strftime(dateStr, DATESZ, "%c", localtime(&mtime));
  insert_key_string(userT, "start_date", dateStr);
  insert_key_double(userDT,"exec_epoch", (double) mtime);

  // user
  char* buff = xalt_getenv("USER");
  insert_key_string(userT, "user", (buff) ? buff : "unknown");

  //Is this a singularity container?
  if (is_directory("/.singularity.d"))
    insert_key_string(userT, "container", "singularity");

  //exec_epoch
  struct stat st;
  mtime = 0L;
  HASH_FIND_STR(*userT, "exec_path", e);
  if (e != NULL)
    {
      if (stat(utstring_body(e->value), &st) != -1)
	mtime = st.st_mtime;
    }
    
  
  //execModify
  strftime(dateStr, DATESZ, "%c", localtime(&mtime));
  insert_key_string(userT, "execModify", dateStr);

  //num_threads
  buff = xalt_getenv("OMP_NUM_THREADS");
  const char* nt          = (buff) ? buff : "1";
  double      num_threads = strtod(nt, (char **) NULL);
  insert_key_double(userDT, "num_threads", num_threads);
}
