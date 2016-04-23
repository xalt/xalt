#include <stdio.h>
#include <string.h>

#include "parseSysLog.h"
#include "SysLogRecord.h"

// XALT_LOGGING V:2 kind:kind key:key syshost:syshost nb:nb idx:idx value:value

bool parseSysLog(const char* buf, SysLogRecord& syslogT, RecordT& recordT)
{
  char *start = strstr(buf, " V:2 ");
  if (start == NULL)
    return false;

  start += 5;
  std::string kind;
  std::string key;
  std::string syshost;
  std::string value;
  long        nb;
  long        idx;

  while(*start != NULL)
    {
        
      // skip leading blanks
      while(isspace(*start++))
        ;

      char* p = strchr(' ');
      if (p == NULL)
        p = start+strlen(start);
      
      if (strcmp(start,"kind:") == 0)
        kind.assign(start, p - start);
      else if (strcmp(start,"key:") == 0)
        key.assign(start, p - start);
      else if (strcmp(start,"syslog:") == 0)
        syshost.assign(start, p - start);
      else if (strcmp(start,"nb:") == 0)
        nb = strtol(start, (char **) NULL, 10);
      else if (strcmp(start,"idx:") == 0)
        idx = strtol(start, (char **) NULL, 10);
      else if (strcmp(start,"value:") == 0)
        value.assign(start, p - start);

      start = p;
    }
      
  bool                    record_stored = false;
  RecordT::const_iterator got           = recordT.find(key);
  if (got == recordT.end())
    syslogT.init(nb, kind, syshost);
  else
    {
      syslogT       = got->second;
      record_stored = true;
    }

  syslogT.addblk(idx, value);
    
  if (syslogT.completed())
    {
      if (record_stored)
        recordT.erase(got);
      return true;
    }
  
  recordT[key] = syslogT;
  return false;
}
