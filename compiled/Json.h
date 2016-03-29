#ifndef JSON_H
#define JSON_H

#include "xalt_types.h"

class Json
{
public:
  Json() { m_s = "{"; }
  ~Json() {};
  void fini();
  void add(const char* name, Table& t);
  void add(const char* name, std::string& value);
  void add(const char* name, Vstring&     v);
  void add(const char* name, Set&         s);
  void add(const char* name, std::vector<Libpair>&     lddA);
  void add_json_string(const char* name, std::string& value);
  std::string& result();

private:
  std::string m_s;
  
};


#endif //JSON_H
