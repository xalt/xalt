#ifndef XALT_MYSQL_UTILS_H
#define XALT_MYSQL_UTILS_H

#include <my_global.h>
#include <mysql.h>
#include <string>
#include "xalt_types.h"

void finish_with_error(MYSQL *conn);
void print_stmt_error(MYSQL_STMT *stmt, const char *message);
void insert_objects(MYSQL* conn, const char* table_name, uint index, std::vector<Libpair>& lddA,
                    std::string& syshost, Table& rmapT);


#endif //XALT_MYSQL_UTILS_H
