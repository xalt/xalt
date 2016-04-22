#ifndef SYSLOGRECORD_H
#define SYSLOGRECORD_H

#include "xalt_types.h"

class SyslogRecord
{
public:
  SyslogRecord(int nblks, std::string& kind, std::string& syshost)
  ~SyslogRecord() {}

  bool          completed()    { return (m_blkcnt >= m_nblks); }
  std::string&  kind()         { return m_kind;                }
  std::string&  syshost()      { return m_syshost;             }
  
  void addblk(int idx, std::string& v);
  void value(std::string v);

private:
  int         m_blkcnt;
  int         m_nblks;
  std::string m_kind;
  std::string m_syshost;
  Vstring     m_blkA;
};

#endif //SYSLOGRECORD_H
