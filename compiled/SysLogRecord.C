#include <stdio.h>
#include "SysLogRecord.h"
#include "base64.h"
#include "zstring.h"


SysLogRecord::SysLogRecord()
  : m_blkcnt(0), m_nblks(-1), m_kind("unknown"), m_syshost("unknown"), m_key("unknown")
{}

void SysLogRecord::init(long nblks, std::string& kind, std::string& syshost, std::string& key)
{
  m_nblkd   = nblks;
  m_kind    = kind;
  m_syshost = syshost;
  m_key     = key;
  m_blkA.reserve(nblk);
}

void SysLogRecord::addblk(long idx, std::string v)
{
  m_blkA[idx] = v;
  m_blkcnt++;
}

void SysLogRecord::value(std::string& jsonStr)
{
  std::string b64;

  b64.assign(m_blkA[0]);
  for (int i = 1; i < m_nblk; ++i)
    b64.append(m_blkA[i]);
  
  std::string zs = base64_decode(b64);
  jsonStr        = decompress_string(zs);
}

void SysLogRecord::prt(const char* name, Vstring& resultA)
{
  const int sz = 15;
  char buf[sz];
  std::string prefix;
  prefix.assign(name);
  prefix.append(" kind:");
  prefix.append(m_kind);
  prefix.append(" key:");
  prefix.append(m_key);
  prefix.append(" syshost:");
  prefix.append(m_syshost);
  prefix.append(" nb:");
  snprintf(buf, sz, "%d",m_nblks);
  prefix.append(buf);

  long idx = 0;
  for (auto it = m_blkA.begin(); it != m_blkA.end(); ++it, ++idx)
    {
      if (it->size() > 0)
        {
          std::string result(prefix);
          result.append(" idx:");
          snprintf(buf, sz, "%d",idx);
          result.append(buf);
          result.append(" value:");
          result.append(*it);
          resultA.push_back(result);
        }
    }
}
