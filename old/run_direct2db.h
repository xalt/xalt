#ifndef RUN_DIRECT2DB_H
#define RUN_DIRECT2DB_H

#include "xalt_types.h"

void run_direct2db(const char* confFn, std::string& usr_cmdline, std::string& hash_id, Table& rmapT,
                   Table& envT, Table& userT, DTable& userDT, Table& recordT, std::vector<Libpair>& lddA);

#endif //RUN_DIRECT2DB_H
