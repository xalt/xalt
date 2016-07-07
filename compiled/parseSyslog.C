#include <stdio.h>
#include <string.h>

#include "parseSyslog.h"
#include "SyslogRecord.h"

// XALT_LOGGING V:2 kind:kind key:key syshost:syshost nb:nb idx:idx value:value

bool parseSyslog(const char* buf, std::string& clusterName, SyslogRecord& syslogT, RecordT& recordT)
{
  char *start = strstr((char *) buf, " V:2 ");
  if (start == NULL)
    return parseSyslogV1(buf, syslogT);

  start += 5;
  std::string kind;
  std::string key;
  std::string syshost;
  std::string value;
  long        nb   = 1;
  long        idx  = 0;
  bool        done = false;

  while(!done)
    {
      // skip leading blanks
      while(isspace(*start))
        start++ ;

      char* end = strchr(start,' ');
      if (end == NULL)
        {
          done = true;
          // remove any trailing whitespace at EOL
          end = start+strlen(start);
          while (isspace(*(end-1)))
            --end;
        }

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

  if (clusterName != ".*" && clusterName != syshost)
    return false;

  bool                    record_stored = false;
  RecordT::const_iterator got           = recordT.find(key);
  if (got == recordT.end())
    syslogT.init(nb, kind, syshost, key);
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

// XALT_LOGGING kind:syshost:value

bool parseSyslogV1(const char* buf, SyslogRecord& syslogT)
{
  long        idx = 0L;
  long        nb  = 1L;
  std::string key="";
  std::string kind;
  std::string syshost;
  std::string value;

  const char* start = strstr((char *) buf,"XALT_LOGGING");
  if (start == NULL)
    return false;

  start += 12;
  while(isspace(*start))
    start++ ;

  const char* p = strchr(start,':');
  if (p)
    kind.assign(start, p - start);

  start = p + 1;
  p = strchr(start,':');
  
  if (p)
    syshost.assign(start, p - start);

  start = p + 1;
  p     = start+strlen(start);

  // remove any trailing spaces.
  while (isspace(*(p-1)))
    --p;

  value.assign(start, p - start);

  syslogT.init(nb, kind, syshost, key);
  syslogT.addblk(idx, value);

  return true;
}
