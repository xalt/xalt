#ifndef F2DB_OPTIONS_H
#define F2DB_OPTIONS_H

#include <string>

class Options
{
public:
  Options(int argc, char **argv);
  ~Options() {}
  bool          deleteFn() { return m_delete; }
  bool          reportFn() { return m_report; }
  bool          timer()    { return m_timer;  }
  std::string&  rmapD()    { return m_rmapD;  }
  std::string&  confFn()   { return m_confFn; }

private:
  bool        m_delete;
  bool        m_report;
  bool        m_timer;
  std::string m_rmapD;
  std::string m_confFn;
};


#endif // F2DB_OPTIONS_H
