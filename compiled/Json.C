#include "Json.h"
#include "xalt_quotestring.h"


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

  for ( auto it = v.begin(); it != v.end(); ++it)
    {
      m_s += "\"";
      m_s += xalt_quotestring((*it).c_str());
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

void Json::add(const char* name, Set& set)
{
  m_s += "\"";
  m_s += name;
  m_s += "\":[";

  for ( auto it = set.begin(); it != set.end(); ++it)
    {
      m_s += "\"";
      m_s += xalt_quotestring((*it).c_str());
      m_s += "\",";
    }
      
  if (m_s.back() == ',')
    m_s.replace(m_s.size()-1,2,"],");
  else
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
  for ( auto it = t.begin(); it != t.end(); ++it )
    {
      const std::string& k = it->first;
      if (k.find("BASH_FUNC_") == 0)
        continue;

      std::string& v = it->second;
      if (v.find("() {") == 0)
        continue;
      
      std::string::size_type idx = v.find("\"");
      if (idx != std::string::npos)
        v.replace(idx,2,"\\\"");

      m_s += "\"";
      m_s += k;
      m_s += "\":\"";
      m_s += xalt_quotestring(v.c_str());
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
void Json::add(const char* name, std::vector<Libpair>&     libA)
{
  m_s += "\"";
  m_s += name;
  m_s += "\":[";
  for ( auto it = libA.begin(); it != libA.end(); ++it)
    {
      std::string& lib = (*it).lib;
      std::string::size_type idx = lib.find("\"");
      if (idx != std::string::npos)
        lib.replace(idx,2,"\\\"");

      m_s += "[\"";
      m_s += xalt_quotestring(lib.c_str());
      m_s += "\",\"";
      m_s += (*it).sha1;
      m_s += "\"],";
    }
  if (m_s.back() == ',')
    m_s.replace(m_s.size()-1,2,"],");
  else
    m_s += "],";
}


std::string& Json::result()
{
  return m_s;
}

