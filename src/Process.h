#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include <string>
#include <vector>


class Process
{
public:
  Process(pid_t pid);
  ~Process() {}

  pid_t        parent()  { return m_parent;  }
  std::string& name()    { return m_name;    }
  std::string& exe();
  void         cmdline(std::vector<std::string> & cmdlineA);

private:
  std::string m_exe;
  std::string m_name;
  pid_t       m_pid;
  pid_t       m_parent;
};



#endif //PROCESS_H
