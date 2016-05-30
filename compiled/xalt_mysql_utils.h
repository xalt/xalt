#ifndef XALT_MYSQL_UTILS_H
#define XALT_MYSQL_UTILS_H

#ifdef HAVE_MYSQL
#  include <my_global.h>
#  include <mysql.h>
#  undef min
#  undef max
#  undef test
#  include <string>
#  include <time.h>
#  include "ConfigParser.h"
#  include "xalt_types.h"

   void finish_with_error(MYSQL *conn);
   void print_stmt_error(MYSQL_STMT *stmt, const char *message);
   void insert_objects(MYSQL* conn, const char* table_name, time_t epoch,  uint index, 
                       std::vector<Libpair>& lddA, std::string& syshost, Table& rmapT);

   MYSQL* xalt_open_mysql_connection(MYSQL* conn, ConfigParser& cp);

#  endif //HAVE_MYSQL

#endif //XALT_MYSQL_UTILS_H
