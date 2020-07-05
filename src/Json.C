#include "Json.h"
#include "xalt_quotestring.h"
#include <string.h>
#include <stdio.h>

Json::Json(Json::Kind kind)
{
  if (kind == Json_TABLE)
    {
      m_s     = "{";
      m_final = "}";
    }
  else
    {
      m_s     = "[";
      m_final = "]";
    }
}

void Json::fini()
{
  if (m_s.back() == ',')
    m_s.replace(m_s.size()-1,1,m_final);
  else
    m_s += m_final;
}

void Json::add(const char* name, std::string& value)
{
  m_s += "\"";
  m_s += name;
  m_s += "\":\"";
  m_s += value;
  m_s += "\",";
}

void Json::add(const char* name, const char * value)
{
  m_s += "\"";
  m_s += name;
  m_s += "\":\"";
  m_s += value;
  m_s += "\",";
}

void Json::add(const char* name, int          value)
{
  char buf[30];
  sprintf(&buf[0],"%d",value);
  m_s += "\"";
  m_s += name;
  m_s += "\":";
  m_s += buf;
  m_s += ",";
}

void Json::add(const char* name, double       value)
{
  char buf[30];
  sprintf(&buf[0],"%g",value);
  m_s += "\"";
  m_s += name;
  m_s += "\":";
  m_s += buf;
  m_s += ",";
}

void Json::add_json_string(const char* name, std::string& value)
{
  m_s += "\"";
  m_s += name;
  m_s += "\":";
  m_s += value;
  m_s += ",";
}

void Json::add(const char* name, Vstring& v)
{
  if (name)
    {
      m_s += "\"";
      m_s += name;
      m_s += "\":[";
    }

  const char* blank0 = "";
  const char* comma  = ",";
  const char* sep   = blank0;

  for ( auto const & it : v)
    {
      m_s += "\"";
      m_s += sep;
      m_s += xalt_quotestring(it.c_str());
      m_s += "\"";
      sep = comma;
    }

  if (name)
    m_s += "],";
}

void Json::add(const char* name, Set& set)
{
  m_s += "\"";
  m_s += name;
  m_s += "\":[";

  const char* blank0 = "";
  const char* comma  = ",";
  const char* sep   = blank0;

  for ( auto const & it : set)
    {
      m_s += "\"";
      m_s += sep;
      m_s += xalt_quotestring(it.c_str());
      m_s += "\"";
      sep = comma;
    }
      
  m_s += "],";
}



void Json::add(const char* name, Table& t)
{
  if (name)
    {
      m_s += "\"";
      m_s += name;
      m_s += "\":{";
    }
  const char* blank0 = "";
  const char* comma  = ",";
  const char* sep   = blank0;

  for ( auto & it : t)
    {
      auto & k = it.first;
      if (k.find("BASH_FUNC_") == 0)
        continue;

      auto & v = it.second;
      if (v.find("() {") == 0)
        continue;
      
      m_s += "\"";
      m_s += sep;
      m_s += k;
      m_s += "\":\"";
      m_s += xalt_quotestring(v.c_str());
      m_s += "\"";
      sep = comma;
    }
  if (name)
    m_s += "},";
}

void Json::add(const char* name, CTable& t)
{
  if (name)
    {
      m_s += "\"";
      m_s += name;
      m_s += "\":{";
    }
  for ( auto const & it : t)
    {
      auto k = it.first;
      if (strcmp("BASH_FUNC_",k) == 0)
        continue;

      auto v = it.second;
      if (strcmp("() {",v) == 0)
        continue;
      
      m_s += "\"";
      m_s += k;
      m_s += "\":\"";
      m_s += xalt_quotestring(v);
      m_s += "\",";
    }
  if (name)
    {
      if (m_s.back() == ',')
        m_s.replace(m_s.size()-1,2,"},");
      else
        m_s += "},";
    }
}

// "processTree":
//     [
//        { "cmd_name":"foo", "cmd_path":"/path/to/foo", "cmdlineA":[ "./foo","-a"]},
//        { "cmd_name":"bar", "cmd_path":"/path/to/bar", "cmdlineA":[ "./bar","-b"]}
//     ],

void Json::add(const char* name, std::vector<ProcessTree>& ptA)
{
  if (name)
    {
      m_s += "\"";
      m_s += name;
      m_s += "\":[";
    }
  for ( auto const & it : ptA)
    {
      char * strbuff = NULL;
      pid_t              pid      = it.pid;
      const std::string& name     = it.name;
      const std::string& path     = it.path;
      const Vstring&     cmdlineA = it.cmdlineA;

      m_s += "\{\"cmd_name\":\"";
      m_s += xalt_quotestring(name.c_str());
      m_s += "\",\"cmd_path\":\"";
      m_s += xalt_quotestring(path.c_str());
      asprintf(&strbuff, "%d", pid);
      m_s += "\",\"pid\":";
      m_s += strbuff;
      m_s += ",\"cmdlineA\":[";
      for ( auto const & jt : cmdlineA)
        {
          m_s += "\"";
          m_s += xalt_quotestring(jt.c_str());
          m_s += "\",";
        }
      if (m_s.back() == ',')
        m_s.replace(m_s.size()-1,2,"]},");
      else
        m_s += "]},";
      if (strbuff)
        free(strbuff);
    }
  if (name)
    {
      if (m_s.back() == ',')
        m_s.replace(m_s.size()-1,2,"],");
      else
        m_s += "],";
    }
}


void Json::add(const char* name, DTable& t)
{
  if (name)
    {
      m_s += "\"";
      m_s += name;
      m_s += "\":{";
    }
  for ( auto const & it : t)
    {
      char * strbuff = NULL;
      auto & k = it.first;
      auto   v = it.second;
      asprintf(&strbuff, "%f", v);
      m_s += "\"";
      m_s += k;
      m_s += "\":";
      m_s += strbuff;
      m_s += ",";
      if (strbuff)
        free(strbuff);
    }
  if (name)
    {
      if (m_s.back() == ',')
        m_s.replace(m_s.size()-1,2,"},");
      else
        m_s += "},";
    }
}

void Json::add(const char* name, std::vector<Libpair>&  libA)
{
  m_s += "\"";
  m_s += name;
  m_s += "\":[";
  for ( auto const & it : libA)
    {
      std::string            lib = it.lib;
      std::string::size_type idx = lib.find("\"");
      if (idx != std::string::npos)
        lib.replace(idx,2,"\\\"");

      m_s += "[\"";
      m_s += xalt_quotestring(lib.c_str());
      m_s += "\",\"";
      m_s += it.sha1;
      m_s += "\"],";
    }
  if (m_s.back() == ',')
    m_s.replace(m_s.size()-1,2,"],");
  else
    m_s += "],";
}

void Json::add(const char* name, int n, const char **A)
{
  m_s += "\"";
  m_s += name;
  m_s += "\":[";
  for (int i = 0; i < n; ++i)
    {
      m_s += "\"";
      m_s += xalt_quotestring(A[i]);
      m_s += "\",";
    }
  if (name)
    {
      if (m_s.back() == ',')
        m_s.replace(m_s.size()-1,2,"],");
      else
        m_s += "],";
    }
}
std::string& Json::result()
{
  return m_s;
}

