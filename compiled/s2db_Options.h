#ifndef S2DB_OPTIONS_H
#define S2DB_OPTIONS_H

#include <string>

class s2db_Options
{
public:
  s2db_Options(int argc, char **argv);
  ~s2db_Options() {}
  bool          timer()      { return m_timer;      }
  std::string&  syshost()    { return m_syshost;    }
  std::string&  syslogFn()   { return m_syslogFn;   }
  std::string&  leftoverFn() { return m_leftoverFn; }
  std::string&  rmapD()      { return m_rmapD;      }
  std::string&  confFn()     { return m_confFn;     }

private:
  bool        m_timer;
  std::string m_syslogFn;
  std::string m_leftoverFn;
  std::string m_rmapD;
  std::string m_confFn;
  std::string m_syshost;
};


#endif // S2DB_OPTIONS_H
