#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>

class ConfigParser
{
public:
  ConfigParser(const char * fn);
  ~ConfigParser() {};

  std::string& host()   { return m_host;   }
  std::string& user()   { return m_user;   }
  std::string& db()     { return m_db;     }
  std::string& passwd() { return m_passwd; }
    
private:
  std::string m_host;
  std::string m_user;
  std::string m_db;
  std::string m_passwd;
};

#endif //CONFIGPARSER_H
