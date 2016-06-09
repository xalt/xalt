#ifndef JSON_H
#define JSON_H

#include "xalt_types.h"

class Json
{
public:
  enum Kind {
    Table,
    Array
  };

  Json(Json::Kind kind = Json::Kind::Table);
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
  char        m_final;
};


#endif //JSON_H
