#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "s2db_Options.h"
#include "epoch.h"
#include "xalt_fgets_alloc.h"
#include "ProgressBar.h"
#include "xalt_types.h"
#include "SyslogRecord.h"
#include "parseSyslog.h"
#include "buildRmapT.h"
#include "parseJsonStr.h"
#include "link_direct2db.h"
#include "run_direct2db.h"

void link_json_to_db(std::string& key, std::string confFn, Table& rmapT, std::string& jsonStr)
{
  Set                  funcSet;
  std::vector<Libpair> libA;
  Vstring              linkLineA;
  Table                resultT;
  
  parseLinkJsonStr(key.c_str(), jsonStr, linkLineA, resultT, libA, funcSet);
  link_direct2db(confFn.c_str(), linkLineA, resultT, libA, funcSet, rmapT);
}

void run_json_to_db(std::string& key, std::string confFn, Table& rmapT, std::string& jsonStr)
{
  std::string              usr_cmdline;
  std::string              hash_id;
  Table                    envT;
  Table                    userT;
  DTable                   userDT;
  Table                    recordT;
  std::vector<Libpair>     libA;
  std::vector<ProcessTree> ptA;

  parseRunJsonStr(key.c_str(), jsonStr, usr_cmdline, hash_id, envT, userT, userDT, recordT, libA, ptA);
  run_direct2db(confFn.c_str(), usr_cmdline, hash_id, rmapT,  envT, userT, userDT, recordT, libA);
}

int main(int argc, char* argv[])
{
  s2db_Options options(argc,argv);
  double t1 = epoch();

  FILE* fp = fopen(options.syslogFn().c_str(), "r");
  if (! fp)
    {
      fprintf(stderr,"Unable to open syslog file: %s. Quiting!\n", options.syslogFn().c_str());
      exit(1);
    }

  
  // Count number of lines in syslog file
  char*  buf = NULL;
  size_t sz  = 0;
  int    num = 0;
  while (xalt_fgets_alloc(fp, &buf, &sz))
    num++;
  free(buf);
  fclose(fp);
  

  int          count  = 0;
  int          lnkCnt = 0;
  int          runCnt = 0;
  ProgressBar  pbar(num);
  SyslogRecord syslogT;
  RecordT      recordT;
  Vstring      fnA;
  Vstring      xlibmapA;
  Table        rmapT;
  buildRmapT(options.rmapD(), rmapT, xlibmapA);
  fnA.push_back(options.leftoverFn());
  fnA.push_back(options.syslogFn());

  for (auto const & it : fnA)
    {
      fp = fopen(it.c_str(), "r");
      if (!fp)
        continue;

      while (xalt_fgets_alloc(fp, &buf, &sz))
        {
          if (strstr(buf, "XALT_LOGGING") == NULL)
            continue;

          //remove trailing newline from buf.
          buf[strlen(buf)-1] = '\0';
            
          bool done = parseSyslog(buf, options.syshost(), syslogT, recordT);
          if (! done)
            continue;

          std::string jsonStr;
          syslogT.value(jsonStr);
          if (syslogT.kind() == "link")
            {
              link_json_to_db(syslogT.key(), options.confFn(), rmapT, jsonStr);
              lnkCnt++;
            }
          else if (syslogT.kind() == "run")
            {
              run_json_to_db(syslogT.key(), options.confFn(), rmapT, jsonStr);
              runCnt++;
            }
          pbar.update(count++);
        }
      fclose(fp);
    }
  free(buf);

  time_t rt = (time_t) (epoch() - t1);
  
  pbar.fini();

  if (options.timer())
    {
      const int dateSz = 100;
      char dateStr[dateSz];
      strftime(dateStr, dateSz, "%T", gmtime(&rt));
      printf("Time: %s\n", dateStr);
    }

  printf("num links: %d, num runs: %d\n", lnkCnt, runCnt);
    
  if (access(options.leftoverFn().c_str(),F_OK) != -1)
    {
      std::string bckFn(options.leftoverFn());
      bckFn.append(".old");
      rename(options.leftoverFn().c_str(), bckFn.c_str());
    }

  if (recordT.size() > 0)
    {
      fp = fopen(options.leftoverFn().c_str(), "w");
      for (auto & it : recordT)
        {
          SyslogRecord& syslogT = it.second;
          Vstring resultA;
          syslogT.prt("XALT_LOGGING V=2",resultA);

          for (auto const jt : resultA)
            fprintf(fp, "%s\n", jt.c_str());
        }
      fclose(fp);
    }
  return 0;
}
