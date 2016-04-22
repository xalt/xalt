#include "SysLogRecord.h"
#include "base64.h"
#include "zstring.h"

SysLogRecord::SysLogRecord(int nblks, std::string& kind, std::string& syshost)
  : m_blkcnt(0), m_nblks(nblks), m_kind(kind), m_syshost(syshost), m_blkA(nblks)
{ }

void addblk(int idx, std::string v)
{
  m_blkA[idx] = v;
  m_blkcnt++;
}

void value(std::string& jsonStr)
{
  std::string b64

  b64.assign(m_blkA[0]);
  for (int i = 1; i < m_nblk; ++i)
    b64.append(m_blkA[i]);
  
  std::string zs = base64_decode(b64);
  jsonStr        = decompress_string(zs);
}
