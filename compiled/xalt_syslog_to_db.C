#include <stdio.h>
#include <string.h>

#include "s2db_Options.h"
#include "epoch.h"
#include "xalt_fgets_alloc.h"
#include "ProgressBar.h"
#include "xalt_types.h"
#include "SysLogRecord.h"

typedef std::unordered_map<std::string, SysLogRecord> RecordT;

int main(int argc, char* argv[])
{
  s2db_Options options(argc,argv);
  double t1 = epoch();

  FILE* fp = fopen(options.syslogFn().c_str(), "r");
  if (! fp)
    {
      fprintf(stderr,"Unable to open syslog file: %s. Quiting!\n", options.syslogFn.c_str());
      exit(1);
    }

  
  // Count number of lines in syslog file
  char*  buf = NULL;
  size_t sz  = 0;
  int    num = 0;
  while (xalt_fgets_alloc(fp, &buf, &sz))
    num++;
  fclose(fp);

  int          lnkCnt = 0;
  int          runCnt = 0;
  ProgressBar  pbar(num);
  SysLogRecord syslogT;
  RecordT      recordT;
  Vstring      xlibmapA;
  Table        rmapT;
  buildRmapT(rmapT, xlibmapA);
  Vstring     fnA;
  fnA.push_back(options.leftoverFn().c_str());
  fnA.push_back(options.syslogFn().c_str());

  for (auto it = fnA.begin(); it != fnA.end(); ++it)
    {
      fp = fopen(it->c_str(), "r");
      if (!fp)
        continue;

      while (xalt_fgets_alloc(fp, &buf, &sz))
        {
          if (strstr(buf, "XALT_LOGGING") == NULL)
            continue;

          bool done = parseSysLog(buf, syslogT, recordT);
          if (! done)
            continue;

          std::string jsonStr;
          syslogT.value(jsonStr)
          if (syslogT.kind() == "link")
            {
              
            
    
  
  


  free(buf);
  return 0;
}
