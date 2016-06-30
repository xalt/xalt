#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/time.h>

#include "run_submission.h"
#define  DATESZ 100

void buildUserT(Options& options, Table& envT, Table& userT, DTable& userDT)
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
  char * strbuf;
  asprintf(&strbuf,"%f",utc);
  userT["currentEpoch"] = strbuf;

  // syshost
  userT["syshost"]      = options.syshost();

  // run_uuid
  userT["run_uuid"]     = options.uuid();

  double runTime        = options.endTime() - options.startTime();
  if (runTime < 0.0)
    runTime = 0.0;

  //num_threads
  buff = getenv("OMP_NUM_THREADS");
  const char* nt          = (buff) ? buff : "1";
  double      num_threads = strtod(nt, (char **) NULL);

  mtime = (time_t) options.startTime();
  strftime(dateStr, DATESZ, "%c", localtime(&mtime));
  userT["start_date"] = dateStr;

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
  
  //execModify
  strftime(dateStr, DATESZ, "%c", localtime(&mtime));
  userT["execModify"] = dateStr;

  //exec_type
  userT["exec_type"] = options.exec_type();

  free(strbuf);

  // Store floating point number in userDT
  userDT["start_time"]   = options.startTime();
  userDT["end_time"]     = options.endTime();
  userDT["run_time"]     = runTime;
  userDT["num_tasks"]    = options.ntasks();
  userDT["currentEpoch"] = utc;
  userDT["num_threads"]  = num_threads;
  userDT["exec_epoch"]   = mtime;

  // Use this translate routine to extract values from the environment to provide standard values.
  // These are stored in userT and userDT.  Later these values are written to the xalt_run table in DB;
  translate(envT, userT, userDT);
}
