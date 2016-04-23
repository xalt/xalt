#ifndef SYSLOGRECORD_H
#define SYSLOGRECORD_H

#include "xalt_types.h"

class SyslogRecord
{
public:
  SyslogRecord();
  void init(long nblks, std::string& kind, std::string& syshost, std::string& key);

  ~SyslogRecord() {}

  bool          completed()    { return (m_blkcnt >= m_nblks); }
  std::string&  kind()         { return m_kind;                }
  std::string&  syshost()      { return m_syshost;             }
  std::string&  key()          { return m_key;                 }
  void addblk(long idx, std::string& v);
  void value(std::string& v);
  void prt(const char* prefix, Vstring& resultA);

private:
  long        m_blkcnt;
  long        m_nblks;
  std::string m_kind;
  std::string m_syshost;
  std::string m_key;
  Vstring     m_blkA;
};

#endif //SYSLOGRECORD_H
