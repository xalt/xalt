#ifndef OPTIONS_H
#define OPTIONS_H

#include <unistd.h>
#include <string>

class Options
{
public:
  Options(int argc, char **argv);
  ~Options() {}
  long          ntasks()      { return m_ntasks;      }
  long          ngpus()       { return m_ngpus;       }
  long          interfaceV()  { return m_interfaceV;  }
  pid_t         ppid()        { return m_ppid;        }
  double        startTime()   { return m_start;       }
  double        endTime()     { return m_end;         }
  double        probability() { return m_probability; }
  std::string&  exec()        { return m_exec;        }
  std::string&  syshost()     { return m_syshost;     }
  std::string&  uuid()        { return m_uuid;        }
  std::string&  userCmdLine() { return m_userCmdLine; }
  std::string&  exec_type()   { return m_exec_type;   }
  std::string&  confFn()      { return m_confFn;      }
  std::string&  path()        { return m_path;        }
  std::string&  ldLibPath()   { return m_ldLibPath;   }

private:
  double      m_start;
  double      m_end;
  double      m_probability;
  long        m_ntasks;
  long        m_ngpus;
  long        m_interfaceV;
  pid_t       m_ppid;
  std::string m_syshost;
  std::string m_uuid;
  std::string m_exec;
  std::string m_userCmdLine;
  std::string m_exec_type;
  std::string m_confFn;
  std::string m_path;
  std::string m_ldLibPath;
};


#endif // OPTIONS_H
