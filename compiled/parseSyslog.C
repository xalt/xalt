#include <stdio.h>
#include <string.h>

#include "parseSyslog.h"
#include "SyslogRecord.h"

// XALT_LOGGING V:2 kind:kind key:key syshost:syshost nb:nb idx:idx value:value

bool parseSyslog(const char* buf, SyslogRecord& syslogT, RecordT& recordT)
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
      while(isspace(*start))
        start++ ;

      char* end = strchr(start,' ');
      if (end == NULL)
        end = start+strlen(start);
      
      char* p = strchr(start,':'); p++;

      if (strncmp(start,"kind:",5) == 0)
        kind.assign(p, end - p); 
      else if (strncmp(start,"key:",4) == 0)
        key.assign(p, end - p);
      else if (strncmp(start,"syshost:",8) == 0)
        syshost.assign(p, end - p);
      else if (strncmp(start,"nb:",3) == 0)
        nb = strtol(p, (char **) NULL, 10);
      else if (strncmp(start,"idx:",4) == 0)
        idx = strtol(p, (char **) NULL, 10);
      else if (strncmp(start,"value:",6) == 0)
        value.assign(p, end - p);

      start = end;
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
