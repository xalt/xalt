#ifndef JSON_H
#define JSON_H

#include "run_submission.h"

class Json
{
public:
  Json() { m_s = "{"; }
  ~Json() {};
  void fini();
  void add(const char* name, table& t);
  void add(const char* name, std::string& value);
  void add(const char* name, std::vector<std::string>& a);
  void add(const char* name, std::vector<Libpair>&     lddA);
  void add_json_string(const char* name, std::string& value);
  std::string& result();

private:
  std::string m_s;
  
};


#endif //JSON_H
