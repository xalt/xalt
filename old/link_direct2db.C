#include <time.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "link_submission.h"
#include "link_direct2db.h"
#include "ConfigParser.h"
#include "Json.h"
#include "xalt_mysql_utils.h"
#include "xalt_utils.h"

#ifdef HAVE_MYSQL

uint select_link_id(MYSQL* conn, std::string& link_uuid)
{
  const char* stmt_sql = "SELECT `link_id` FROM `xalt_link` WHERE `uuid` = ? LIMIT 1";

  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt for selecting link_id",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND param[1], result[1];
  memset((void *) param,  0, sizeof(param));
  memset((void *) result, 0, sizeof(result));

  // STRING PARAM[0] link_uuid;
  std::string::size_type len_link_uuid = link_uuid.size();
  param[0].buffer_type   = MYSQL_TYPE_STRING;
  param[0].buffer        = (void *) link_uuid.c_str();
  param[0].buffer_length = link_uuid.capacity();
  param[0].length        = &len_link_uuid;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for selecting link_id",__FILE__,__LINE__);
      exit(1);
    }
      
  // UNSIGNED INT RESULT[0] link_id;
  uint                    link_id;
  result[0].buffer_type   = MYSQL_TYPE_LONG;
  result[0].buffer        = (void *) &link_id;
  result[0].is_unsigned   = 1;

  if (mysql_stmt_bind_result(stmt, result))
    {
      print_stmt_error(stmt, "Could not bind paramaters for selecting link_id",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for selecting link_id",__FILE__,__LINE__);
      exit(1);
    }

  // This function sets run_id if it is successful. It also sets iret to zero if it finds link_id;
  int iret = mysql_stmt_fetch(stmt); 

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for selecting link_id",__FILE__,__LINE__);
      exit(1);
    }

  // Clean up!
  mysql_stmt_free_result(stmt);
  return (iret == MYSQL_NO_DATA) ? 0 : link_id;
}

void insert_xalt_link(MYSQL* conn, Table& resultT, Table& rmapT, Vstring& linklineA, uint* link_id)
{

  const char* stmt_sql = "INSERT into xalt_link VALUES (NULL,?,?,?, ?,?,?, COMPRESS(?),?,?, ?,?)";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt for selecting run_id",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND param[11];
  memset((void *) param,  0, sizeof(param));
  
  // STRING PARAM[0] link_uuid
  std::string& link_uuid = resultT["uuid"];
  std::string::size_type len_link_uuid = link_uuid.size();
  param[0].buffer_type   = MYSQL_TYPE_STRING;
  param[0].buffer        = (void *) link_uuid.c_str();
  param[0].buffer_length = link_uuid.capacity();
  param[0].length        = &len_link_uuid;

  // STRING PARAM[1] hash_id
  std::string& hash_id = resultT["uuid"];
  std::string::size_type len_hash_id = hash_id.size();
  param[1].buffer_type   = MYSQL_TYPE_STRING;
  param[1].buffer        = (void *) hash_id.c_str();
  param[1].buffer_length = hash_id.capacity();
  param[1].length        = &len_hash_id;

  // DATETIME PARAM[2] date 
  MYSQL_TIME my_datetime;
  double     bTimeD       = strtod(resultT["build_epoch"].c_str(), NULL);
  time_t     bTimeI       = (time_t) bTimeD;
  struct tm* bTime        = localtime(&bTimeI);
  my_datetime.year        = bTime->tm_year + 1900;
  my_datetime.month       = bTime->tm_mon  + 1;
  my_datetime.day         = bTime->tm_mday;
  my_datetime.hour        = bTime->tm_hour;
  my_datetime.minute      = bTime->tm_min;
  my_datetime.second      = bTime->tm_sec;
  my_datetime.second_part = 0;
  param[2].buffer_type    = MYSQL_TYPE_DATETIME;
  param[2].buffer         = &my_datetime;

  // STRING PARAM[3] link_prgm
  std::string& link_prgm = resultT["link_program"];
  std::string::size_type len_link_prgm = link_prgm.size();
  param[3].buffer_type   = MYSQL_TYPE_STRING;
  param[3].buffer        = (void *) link_prgm.c_str();
  param[3].buffer_length = link_prgm.capacity();
  param[3].length        = &len_link_prgm;
  
  // STRING PARAM[4] link_path
  std::string& link_path = resultT["link_path"];
  std::string::size_type len_link_path = link_path.size();
  param[4].buffer_type   = MYSQL_TYPE_STRING;
  param[4].buffer        = (void *) link_path.c_str();
  param[4].buffer_length = link_path.capacity();
  param[4].length        = &len_link_path;
  
  // STRING PARAM[5] link_module_name
  const int                  module_name_sz = 64;
  char                       module_name[module_name_sz];
  std::string::size_type     len_module_name = 0;      // set length in loop
  my_bool                    module_name_null_flag;
  param[5].buffer_type   = MYSQL_TYPE_STRING;
  param[5].buffer        = (void *) &module_name[0];
  param[5].buffer_length = module_name_sz;
  param[5].is_null       = &module_name_null_flag;
  param[5].length        = &len_module_name;
  if (path2module(link_path.c_str(), rmapT, module_name,module_name_sz))
    {
      module_name_null_flag = 0;
      len_module_name       = strlen(module_name);
    }
  else
    module_name_null_flag = 1;

  // STRING PARAM[6] link line (json string) (db compresses object)
  Json json(Json::Kind::Json_ARRAY);
  json.add(NULL,linklineA);
  json.fini();
  std::string linkline   = json.result();
  std::string::size_type len_linkline = linkline.size();
  param[6].buffer_type   = MYSQL_TYPE_STRING;
  param[6].buffer        = (void *) linkline.c_str();
  param[6].buffer_length = linkline.capacity();
  param[6].length        = &len_linkline;
  
  // STRING PARAM[7] build_user
  std::string& build_user = resultT["build_user"];
  std::string::size_type len_build_user = build_user.size();
  param[7].buffer_type   = MYSQL_TYPE_STRING;
  param[7].buffer        = (void *) build_user.c_str();
  param[7].buffer_length = build_user.capacity();
  param[7].length        = &len_build_user;

  // STRING PARAM[8] build_syshost
  std::string& build_syshost = resultT["build_syshost"];
  std::string::size_type len_build_syshost = build_syshost.size();
  param[8].buffer_type   = MYSQL_TYPE_STRING;
  param[8].buffer        = (void *) build_syshost.c_str();
  param[8].buffer_length = build_syshost.capacity();
  param[8].length        = &len_build_syshost;
  
  // double PARAM[9] build_epoch
  double build_epoch     = strtod(resultT["build_epoch"].c_str(), NULL);
  param[9].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[9].buffer        = (void *) &build_epoch;

  // STRING PARAM[10] exec_path
  std::string& exec_path   = resultT["exec_path"];
  std::string::size_type len_exec_path = exec_path.size();
  param[10].buffer_type   = MYSQL_TYPE_STRING;
  param[10].buffer        = (void *) exec_path.c_str();
  param[10].buffer_length = exec_path.capacity();
  param[10].length        = &len_exec_path;
  
  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for insert xalt_link",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for insert xalt_link",__FILE__,__LINE__);
      exit(1);
    }

  *link_id = (unsigned int) mysql_stmt_insert_id(stmt);

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for insert xalt_link",__FILE__,__LINE__);
      exit(1);
    }
}

void insert_functions(MYSQL* conn, time_t epoch, Set& funcSet, uint link_id)
{
  //************************************************************
  // build SELECT obj_id INTO xalt_object stmt
  //************************************************************

  const char* stmt_sql_s = "SELECT func_id FROM xalt_function WHERE function_name=? LIMIT 1";

  MYSQL_STMT *stmt_s = mysql_stmt_init(conn);
  if (!stmt_s)
    {
      print_stmt_error(stmt_s, "mysql_stmt_init(), out of memmory",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_s, stmt_sql_s, strlen(stmt_sql_s)))
    {
      print_stmt_error(stmt_s, "Could not prepare stmt_s for select func_id",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND param_s[1], result_s[1];
  memset((void *) param_s,  0, sizeof(param_s));
  memset((void *) result_s, 0, sizeof(result_s));

  const int funcNameSz = 512;
  char funcName[funcNameSz];

  // STRING PARAM_S[0] funcName;
  std::string::size_type len_funcName = 0;          // set length later in loop.
  param_s[0].buffer_type   = MYSQL_TYPE_STRING;
  param_s[0].buffer        = (void *) &funcName;
  param_s[0].buffer_length = funcNameSz;
  param_s[0].length        = &len_funcName;

  if (mysql_stmt_bind_param(stmt_s, param_s))
    {
      print_stmt_error(stmt_s, "Could not bind paramaters for selecting obj_id(1)",__FILE__,__LINE__);
      exit(1);
    }
      
  // UNSIGNED INT RESULT_S func_id;
  uint func_id;
  result_s[0].buffer_type   = MYSQL_TYPE_LONG;
  result_s[0].buffer        = (void *) &func_id;
  result_s[0].is_unsigned   = 1;

  if (mysql_stmt_bind_result(stmt_s, result_s))
    {
      print_stmt_error(stmt_s, "Could not bind paramaters for selecting obj_id(2)",__FILE__,__LINE__);
      exit(1);
    }

  //************************************************************
  // build INSERT into xalt_function stmt
  //************************************************************

  const char* stmt_sql_i = "INSERT INTO xalt_function VALUES (NULL,?)";
  MYSQL_STMT *stmt_i = mysql_stmt_init(conn);
  if (!stmt_i)
    {
      print_stmt_error(stmt_i, "mysql_stmt_init(), out of memmory(2)",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_i, stmt_sql_i, strlen(stmt_sql_i)))
    {
      print_stmt_error(stmt_i, "Could not prepare stmt_i for insert into xalt_function",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND param_i[1];
  memset((void *) param_i,  0, sizeof(param_i));

  // STRING PARAM_I[0] funcName
  param_i[0].buffer_type   = MYSQL_TYPE_STRING;
  param_i[0].buffer        = (void *) &funcName;
  param_i[0].buffer_length = funcNameSz;
  param_i[0].length        = &len_funcName;

  if (mysql_stmt_bind_param(stmt_i, param_i))
    {
      print_stmt_error(stmt_i, "Could not bind paramaters for inserting into xalt_object",__FILE__,__LINE__);
      exit(1);
    }
  
  //************************************************************
  // "INSERT into join_link_function VALUES (NULL, ?, ?, ?)"
  //************************************************************

  const char* stmt_sql_ii = "INSERT INTO join_link_function VALUES(NULL, ?, ?, ?) "
                            "ON DUPLICATE KEY UPDATE func_id = ?, link_id = ?";

  MYSQL_STMT *stmt_ii     = mysql_stmt_init(conn);
  if (!stmt_ii)
    {
      print_stmt_error(stmt_ii, "mysql_stmt_init(), out of memmory(3)",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_ii, stmt_sql_ii, strlen(stmt_sql_ii)))
    {
      print_stmt_error(stmt_ii, "Could not prepare stmt_ii for insert into join_link_function",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND param_ii[5];
  memset((void *) param_ii,  0, sizeof(param_ii));

  // UINT PARAM_II[0] func_id
  param_ii[0].buffer_type   = MYSQL_TYPE_LONG;
  param_ii[0].buffer        = (void *) &func_id;
  param_ii[0].is_unsigned   = 1;

  // UINT PARAM_II[1] link_id
  param_ii[1].buffer_type   = MYSQL_TYPE_LONG;
  param_ii[1].buffer        = (void *) &link_id;
  param_ii[1].is_unsigned   = 1;

  // DATE PARAM_II[2] date
  MYSQL_TIME my_datetime;
  struct tm* curr_time      = localtime(&epoch);
  my_datetime.year          = curr_time->tm_year + 1900;
  my_datetime.month         = curr_time->tm_mon  + 1;
  my_datetime.day           = curr_time->tm_mday;
  my_datetime.hour          = 0;
  my_datetime.minute        = 0;
  my_datetime.second        = 0;
  my_datetime.second_part   = 0;
  param_ii[2].buffer_type   = MYSQL_TYPE_DATE;
  param_ii[2].buffer        = &my_datetime;

  // UINT PARAM_II[3] func_id
  param_ii[3].buffer_type   = MYSQL_TYPE_LONG;
  param_ii[3].buffer        = (void *) &func_id;
  param_ii[3].is_unsigned   = 1;

  // UINT PARAM_II[4] link_id
  param_ii[4].buffer_type   = MYSQL_TYPE_LONG;
  param_ii[4].buffer        = (void *) &link_id;
  param_ii[4].is_unsigned   = 1;

  if (mysql_stmt_bind_param(stmt_ii, param_ii))
    {
      print_stmt_error(stmt_ii, "Could not bind paramaters for inserting into xalt_object",__FILE__,__LINE__);
      exit(1);
    }

  for (auto const & it : funcSet)
    {
      len_funcName = it.size();
      strcpy(&funcName[0], it.c_str());

      // "SELECT func_id ..."
      if (mysql_stmt_execute(stmt_s))
        {
          print_stmt_error(stmt_s, "Could not execute stmt for selecting func_id",__FILE__,__LINE__);
          exit(1);
        }
      if (mysql_stmt_store_result(stmt_s))
        {
          print_stmt_error(stmt_s, "Could not mysql_stmt_store_result() selecting func_id",__FILE__,__LINE__);
          exit(1);
        }
      
      // mysql_stmt_fetch(stmt_s) will return 0 if successful.  That means it found func_id
      if (mysql_stmt_fetch(stmt_s) != 0)
        {
          // "INSERT INTO xalt_function ..."
          if (mysql_stmt_execute(stmt_i))
            {
              print_stmt_error(stmt_i, "Could not execute stmt for inserting into xalt_object",__FILE__,__LINE__);
              exit(1);
            }
          func_id = (uint) mysql_stmt_insert_id(stmt_i);
        }

      // "INSERT INTO join_link_function ..."
      if (mysql_stmt_execute(stmt_ii))
        {
          print_stmt_error(stmt_ii, "Could not execute stmt for inserting into join_link_function",__FILE__,__LINE__);
          exit(1);
        }
    }

  mysql_stmt_free_result(stmt_s);
  if (mysql_stmt_close(stmt_s))
    {
      print_stmt_error(stmt_s, "Could not close stmt for selecting func_id",__FILE__,__LINE__);
      exit(1);
    }
  if (mysql_stmt_close(stmt_i))
    {
      print_stmt_error(stmt_i, "Could not close stmt for inserting into xalt_function",__FILE__,__LINE__);
      exit(1);
    }
  if (mysql_stmt_close(stmt_ii))
    {
      print_stmt_error(stmt_ii, "Could not close stmt for inserting into join_link_function",__FILE__,__LINE__);
      exit(1);
    }
}

void link_direct2db(const char* confFn, Vstring& linklineA, Table& resultT, std::vector<Libpair>& libA, Set& funcSet, Table& rmapT)
{
  ConfigParser cp(confFn);

  MYSQL *conn = mysql_init(NULL);

  if (conn == NULL)
    {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
    }  

  // Note the interface is in autocommit mode.  This should probably change to transaction for the entire link record.

  /* (1) Open DB */
  if (xalt_open_mysql_connection(conn, cp) == NULL)
    finish_with_error(conn);

  // Start the transaction
  mysql_query(conn, "START TRANSACTION");

  // Check to see if we have already stored this link record.  If so then return.
  std::string link_uuid = resultT["uuid"];
  if (select_link_id(conn, link_uuid) != 0)
    return;
  
  uint link_id;
  time_t build_epoch = (time_t) strtod(resultT["build_epoch"].c_str(), NULL);
  insert_xalt_link(conn, resultT, rmapT, linklineA, &link_id);
  insert_objects(conn, "join_link_object", build_epoch, link_id, libA, resultT["build_syshost"], rmapT);
  insert_functions(conn, build_epoch, funcSet, link_id);
  mysql_query(conn, "COMMIT");
  mysql_close(conn);
}

#else
void link_direct2db(const char* confFn, Vstring& linklineA, Table& resultT, std::vector<Libpair>& libA, Set& funcSet, Table& rmapT)
{
  fprintf(stderr,"This version of XALT was not built with MySQL support.\n"
          "You can not use the direct2db transmission style.  Aborting!\n");
  exit(1);
}
#endif //HAVE_MYSQL
