#include "run_submission.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <stdio.h>

#include <iostream>
#include <sys/time.h>
#define  DATESZ 100

void buildUserT(Options& options, table& userT)
{
  
  time_t mtime;
  char   dateStr[DATESZ];
  char   path[PATH_MAX];

  // CWD
  getcwd(path,PATH_MAX);
  userT["cwd"] = path;

  // Epoch
  timeval tm;
  gettimeofday(&tm, NULL);
  double utc = tm.tv_sec + tm.tv_usec*1.e-6;
  char * buff;
  asprintf(&buff,"%f",utc);
  userT["currentEpoch"] = buff;

  // syshost
  userT["syshost"]      = options.syshost();

  // run_uuid
  userT["run_uuid"]         = options.uuid();

  // exit status
  userT["exit_status"]  = "0";
  
  // start_time, end_time, run_time, start_date
  asprintf(&buff,"%f",options.startTime());
  userT["start_time"]   = buff;
  asprintf(&buff,"%f",options.endTime());
  userT["end_time"]     = buff;
  double runTime        = options.endTime() - options.startTime();
  if (runTime < 0.0)
    runTime = 0.0;
  asprintf(&buff,"%f", runTime);
  userT["run_time"]     = buff;
  mtime = (time_t) options.startTime();
  strftime(dateStr, DATESZ, "%c", localtime(&mtime));
  userT["start_date"] = dateStr;

  //num_tasks
  asprintf(&buff, "%ld", options.ntasks());
  userT["num_tasks"] = buff;

  //num_threads
  buff = getenv("OMP_NUM_THREADS");
  userT["num_threads"] = (buff) ? buff : "1";
    
  //user
  buff = getenv("USER");
  userT["user"] = (buff) ? buff : "unknown";

  //exec_path
  userT["exec_path"] = options.exec();

  //exec_epoch
  struct stat st;
  mtime = 0L;
  if (stat(options.exec().c_str(), &st) != -1)
    mtime = st.st_mtime;
  asprintf(&buff, "%ld", mtime);
  userT["exec_epoch"] = buff;
  
  //execModify
  strftime(dateStr, DATESZ, "%c", localtime(&mtime));
  userT["execModify"] = dateStr;

  //exec_type
  userT["exec_type"] = options.exec_type();

  free(buff);
}
