#include <stdio.h>
#include <stdlib.h>
#include "SyslogRecord.h"
#include "base64.h"
#include "zstring.h"


SyslogRecord::SyslogRecord()
  : m_blkcnt(0), m_nblks(-1), m_kind("unknown"), m_syshost("unknown"), m_key("unknown")
{}

void SyslogRecord::init(long nblks, std::string& kind, std::string& syshost, std::string& key)
{
  m_blkcnt  = 0;
  m_nblks   = nblks;
  m_kind    = kind;
  m_syshost = syshost;
  m_key     = key;
  m_blkA.reserve(m_nblks);
  for (int i = 0; i < m_nblks; i++)
    m_blkA.push_back("");
}

void SyslogRecord::addblk(long idx, std::string& v)
{
  m_blkA[idx] = v;
  m_blkcnt++;
}

void SyslogRecord::value(std::string& jsonStr)
{
  int         lenStr;
  std::string b64;

  b64.assign(m_blkA[0]);
  for (int i = 1; i < m_nblks; ++i)
    b64.append(m_blkA[i]);
  
  unsigned char  *str = base64_decode(b64.c_str(), b64.size(), &lenStr);
  std::string     zs  = reinterpret_cast<char*>(str);
  free(str);
  jsonStr         = decompress_string(zs);
}

void SyslogRecord::prt(const char* name, Vstring& resultA)
{
  const int sz = 15;
  char buf[sz];
  std::string prefix;
  std::string result;
  prefix.assign(name);
  prefix.append(" kind:");
  prefix.append(m_kind);
  prefix.append(" key:");
  prefix.append(m_key);
  prefix.append(" syshost:");
  prefix.append(m_syshost);
  prefix.append(" nb:");
  snprintf(buf, sz, "%ld",m_nblks);
  prefix.append(buf);

  long idx = 0;
  for (auto const & it : m_blkA)
    {
      if (it.size() > 0)
        {
          result.assign(prefix);
          result.append(" idx:");
          snprintf(buf, sz, "%ld",idx);
          result.append(buf);
          result.append(" value:");
          result.append(it);
          resultA.push_back(result);
        }
      ++idx;
    }
}
