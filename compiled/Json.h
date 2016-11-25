#ifndef JSON_H
#define JSON_H

#include "xalt_types.h"

class Json
{
public:
  enum Kind {
    Json_TABLE,
    Json_ARRAY
  };

  Json(Json::Kind kind = Json::Kind::Json_TABLE);
  ~Json() {};
  void fini();
  void add(const char* name, Table&                    t);
  void add(const char* name, DTable&                   t);
  void add(const char* name, std::string&              value);
  void add(const char* name, const char*               value);
  void add(const char* name, int                       value);
  void add(const char* name, Vstring&                  v);
  void add(const char* name, Set&                      s);
  void add(const char* name, std::vector<Libpair>&     lddA);
  void add(const char* name, std::vector<ProcessTree>& ptA);
  void add(const char* name, int n,     const char   **A);
  void add_json_string(const char* name, std::string&  value);
  std::string& result();

private:
  std::string m_s;
  std::string m_final;
};


#endif //JSON_H
