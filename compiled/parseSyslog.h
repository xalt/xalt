#ifndef PARSESYSLOG_H
#define PARSESYSLOG_H

#include "xalt_types.h"
#include "SyslogRecord.h"
typedef std::unordered_map<std::string, SyslogRecord> RecordT;
bool parseSyslog(const char* buf, SyslogRecord& syslogT, RecordT& recordT);


#endif  //PARSESYSLOG_H
