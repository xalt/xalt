#ifndef PARSESYSLOG_H
#define PARSESYSLOG_H

#include "xalt_types.h"
#include "SyslogRecord.h"
typedef std::unordered_map<std::string, SyslogRecord> RecordT;
bool parseSyslog(const char* buf, std::string& clusterName, SyslogRecord& syslogT, RecordT& recordT);
bool parseSyslogV1(const char* buf, SyslogRecord& syslogT);


#endif  //PARSESYSLOG_H
