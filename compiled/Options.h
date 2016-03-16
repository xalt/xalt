#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

class Options
{
public:
  Options(int argc, char **argv);
  ~Options() {}
  long          ntasks()      { return m_ntasks;      }
  double        startTime()   { return m_start;       }
  double        endTime()     { return m_end;         }
  std::string&  exec()        { return m_exec;        }
  std::string&  syshost()     { return m_syshost;     }
  std::string&  uuid()        { return m_uuid;        }
  std::string&  userCmdLine() { return m_userCmdLine; }
  std::string&  exec_type()   { return m_exec_type;   }
  std::string&  confFn()      { return m_confFn;      }

private:
  double      m_start;
  double      m_end;
  long        m_ntasks;
  std::string m_syshost;
  std::string m_uuid;
  std::string m_exec;
  std::string m_userCmdLine;
  std::string m_exec_type;
  std::string m_confFn;
};


#endif // OPTIONS_H
