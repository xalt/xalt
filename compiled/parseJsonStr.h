#ifndef PARSEJSONSTR_H
#define PARSEJSONSTR_H

#include "xalt_types.h"
void parseRunJsonStr(std::string& jsonStr, std::string& usr_cmdline, std::string& hash_id, Table& envT, Table& userT,
                     Table& recordT, std::vector<Libpair>& libA);
void parseLinkJsonStr(std::string& jsonStr, Vstring& linklineA, Table& resultT, std::vector<Libpair>& libA, Set& funcSet);



#endif // PARSEJSONSTR_H
