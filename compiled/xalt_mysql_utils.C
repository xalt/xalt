#include <string.h>
#include "xalt_utils.h"
#include "xalt_mysql_utils.h"
#include "xalt_types.h"
#include <time.h>   // for nanosleep

static const int issueWarning = 1;

void finish_with_error(MYSQL *conn)
{
  if (issueWarning)
    {
      fprintf(stderr, "RTM %s\n", mysql_error(conn));
      mysql_close(conn);
    }
  exit(1);        
}

void print_stmt_error(MYSQL_STMT *stmt, const char *message)
{
  fprintf(stderr,"%s\n", message);
  if (stmt != NULL)
    {
      fprintf(stderr,"Error %u (%s): %s\n",
              mysql_stmt_errno(stmt),
              mysql_stmt_sqlstate(stmt),
              mysql_stmt_error(stmt));
    }
}

void object_type(const char* path, char* result)
{
  std::string s(path);
  std::string extA[] = {"a", "o", "so", "--"};
  int nExtA = sizeof(extA)/sizeof(extA[0]);

  int resultIdx = 3;

  while (1)
    {
      size_t idx = s.rfind(".");
      if (idx == std::string::npos)
        break;
      std::string ext(s, idx+1, std::string::npos);
      
      for (int i = 0; i < nExtA; ++i)
        {
          if (ext == extA[i])
            {
              resultIdx = i;
              goto done;
            }
        }
      s.erase(idx, std::string::npos);
    }
 done:
  strcpy(result, extA[resultIdx].c_str());
}

void insert_objects(MYSQL* conn, const char* table_name, uint index, std::vector<Libpair>& lddA,
                    std::string& syshost, Table& rmapT)
{

  //************************************************************
  // build SELECT obj_id INTO xalt_object stmt
  //************************************************************

  const char* stmt_sql_s = "SELECT obj_id FROM xalt_object WHERE hash_id=? AND object_path=? AND syshost=? limit 1";

  MYSQL_STMT *stmt_s = mysql_stmt_init(conn);
  if (!stmt_s)
    {
      print_stmt_error(stmt_s, "mysql_stmt_init(), out of memmory");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_s, stmt_sql_s, strlen(stmt_sql_s)))
    {
      print_stmt_error(stmt_s, "Could not prepare stmt_s for select obj_id");
      exit(1);
    }

  MYSQL_BIND param_s[3], result_s[1];
  memset((void *) param_s,  0, sizeof(param_s));
  memset((void *) result_s, 0, sizeof(result_s));

  const int hash_id_sz = 41;
  char hash_id[hash_id_sz];

  const int objSz = 2048;
  char object_path[objSz];

  // STRING PARAM_S[0] hash_id;
  std::string::size_type len_hash_id = 0;          // set length later in loop.
  param_s[0].buffer_type   = MYSQL_TYPE_STRING;
  param_s[0].buffer        = (void *) &hash_id[0];
  param_s[0].buffer_length = hash_id_sz;
  param_s[0].length        = &len_hash_id;

  // STRING PARAM_S[1] object_path
  std::string::size_type len_object_path = 0;      // set length later in loop.
  param_s[1].buffer_type   = MYSQL_TYPE_STRING;
  param_s[1].buffer        = (void *) &object_path[0];
  param_s[1].buffer_length = objSz;
  param_s[1].length        = &len_object_path;

  // STRING PARAM_S[2] syshost
  std::string::size_type len_syshost = syshost.size();
  param_s[2].buffer_type   = MYSQL_TYPE_STRING;
  param_s[2].buffer        = (void *) syshost.c_str();
  param_s[2].buffer_length = syshost.capacity();
  param_s[2].length        = &len_syshost;

  if (mysql_stmt_bind_param(stmt_s, param_s))
    {
      print_stmt_error(stmt_s, "Could not bind paramaters for selecting obj_id(1)");
      exit(1);
    }
      
  // UNSIGNED INT RESULT_S obj_id;
  uint obj_id;
  result_s[0].buffer_type   = MYSQL_TYPE_LONG;
  result_s[0].buffer        = (void *) &obj_id;
  result_s[0].is_unsigned   = 1;
  result_s[0].length        = 0;

  if (mysql_stmt_bind_result(stmt_s, result_s))
    {
      print_stmt_error(stmt_s, "Could not bind paramaters for selecting obj_id(2)");
      exit(1);
    }

  //************************************************************
  // build INSERT into xalt_object stmt
  //************************************************************

  const char* stmt_sql_i = "INSERT INTO xalt_object VALUES (NULL,?,?,?,?,?,?)";
  MYSQL_STMT *stmt_i = mysql_stmt_init(conn);
  if (!stmt_i)
    {
      print_stmt_error(stmt_i, "mysql_stmt_init(), out of memmory(2)");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_i, stmt_sql_i, strlen(stmt_sql_i)))
    {
      print_stmt_error(stmt_i, "Could not prepare stmt_i for insert into xalt_object");
      exit(1);
    }

  MYSQL_BIND param_i[6];
  memset((void *) param_i,  0, sizeof(param_i));

  // STRING PARAM_I[0] object_path
  param_i[0].buffer_type   = MYSQL_TYPE_STRING;
  param_i[0].buffer        = (void *) &object_path[0];
  param_i[0].buffer_length = objSz;
  param_i[0].length        = &len_object_path;

  // STRING PARAM_I[1] syshost
  param_i[1].buffer_type   = MYSQL_TYPE_STRING;
  param_i[1].buffer        = (void *) syshost.c_str();
  param_i[1].buffer_length = syshost.capacity();
  param_i[1].length        = &len_syshost;

  // STRING PARAM_I[2] hash_id
  param_i[2].buffer_type   = MYSQL_TYPE_STRING;
  param_i[2].buffer        = (void *) &hash_id[0];
  param_i[2].buffer_length = hash_id_sz;
  param_i[2].length        = &len_hash_id;

  // STRING PARAM_I[3] module_name
  const int                  module_name_sz = 64;
  char                       module_name[module_name_sz];
  std::string::size_type     len_module_name = 0;      // set length in loop
  my_bool                    module_name_null_flag;
  param_i[3].buffer_type   = MYSQL_TYPE_STRING;
  param_i[3].buffer        = (void *) &module_name[0];
  param_i[3].buffer_length = module_name_sz;
  param_i[3].is_null       = &module_name_null_flag;
  param_i[3].length        = &len_module_name;

  // TIMESTAMP PARAM_I[4] timestamp
  MYSQL_TIME my_datetime;
  time_t clock;
  (void ) time(&clock);
  struct tm* curr_time    = localtime(&clock);
  my_datetime.year        = curr_time->tm_year + 1900;
  my_datetime.month       = curr_time->tm_mon  + 1;
  my_datetime.day         = curr_time->tm_mday;
  my_datetime.hour        = curr_time->tm_hour;
  my_datetime.minute      = curr_time->tm_min;
  my_datetime.second      = curr_time->tm_sec;
  my_datetime.second_part = 0;
  param_i[4].buffer_type  = MYSQL_TYPE_DATETIME;
  param_i[4].buffer       = &my_datetime;

  // STRING PARAM_I[5] lib_type
  const int lib_type_sz = 3;
  char lib_type[lib_type_sz];

  std::string::size_type len_lib_type = 0;      // set length in loop.
  param_i[5].buffer_type   = MYSQL_TYPE_STRING;
  param_i[5].buffer        = (void *) &lib_type[0];
  param_i[5].buffer_length = lib_type_sz;
  param_i[5].length        = &len_lib_type;

  if (mysql_stmt_bind_param(stmt_i, param_i))
    {
      print_stmt_error(stmt_i, "Could not bind paramaters for inserting into xalt_object");
      exit(1);
    }
      

  //************************************************************
  // "INSERT into <TABLE_NAME> VALUES (NULL, ?, ?)"
  //************************************************************

  std::string s2("INSERT INTO ");
  s2.append(table_name);
  s2.append(" VALUES (NULL,?,?)");
  const char* stmt_sql_ii = s2.c_str();

  MYSQL_STMT *stmt_ii     = mysql_stmt_init(conn);
  if (!stmt_ii)
    {
      print_stmt_error(stmt_ii, "mysql_stmt_init(), out of memmory(3)");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_ii, stmt_sql_ii, strlen(stmt_sql_ii)))
    {
      print_stmt_error(stmt_ii, "Could not prepare stmt_ii for insert into <table_name>");
      exit(1);
    }

  MYSQL_BIND param_ii[2];
  memset((void *) param_ii,  0, sizeof(param_ii));

  // UINT PARAM_II[0] obj_id
  param_ii[0].buffer_type   = MYSQL_TYPE_LONG;
  param_ii[0].buffer        = (void *) &obj_id;
  param_ii[0].buffer_length = 0;
  param_ii[0].is_unsigned   = 1;

  // UINT PARAM_II[1] index
  param_ii[1].buffer_type   = MYSQL_TYPE_LONG;
  param_ii[1].buffer        = (void *) &index;
  param_ii[1].buffer_length = 0;
  param_ii[1].is_unsigned   = 1;

  if (mysql_stmt_bind_param(stmt_ii, param_ii))
    {
      print_stmt_error(stmt_ii, "Could not bind paramaters for inserting into xalt_object");
      exit(1);
    }
      
  
  
  
  for ( auto it = lddA.begin(); it != lddA.end(); ++it)
    {
      len_object_path = (*it).lib.size();
      strcpy(object_path,(*it).lib.c_str());

      len_hash_id     = (*it).sha1.size();
      strcpy(hash_id,(*it).sha1.c_str());

      // "SELECT obj_id ..."
      if (mysql_stmt_execute(stmt_s))
        {
          print_stmt_error(stmt_s, "Could not execute stmt for selecting obj_id");
          exit(1);
        }
      if (mysql_stmt_store_result(stmt_s))
        {
          print_stmt_error(stmt_s, "Could not mysql_stmt_store_result() selecting obj_id");
          exit(1);
        }
      


      // mysql_stmt_fetch(stmt_s) will return 0 if successful.  That means it found obj_id
      if (mysql_stmt_fetch(stmt_s) != 0)  
        {
          // If here then the object is NOT in the db so store it and compute obj_id
          if (path2module(object_path, rmapT, module_name, module_name_sz))
            {
              module_name_null_flag = 0;
              len_module_name = strlen(module_name);
            }
          else
            {
              module_name_null_flag = 1;
              len_module_name = 0;
            }

          object_type(object_path, &lib_type[0]);
          len_lib_type = strlen(lib_type);

          // "INSERT INTO xalt_object ..."
          if (mysql_stmt_execute(stmt_i))
            {
              print_stmt_error(stmt_i, "Could not execute stmt for inserting into xalt_object");
              exit(1);
            }
          obj_id = (uint)  mysql_stmt_insert_id(stmt_i);
        }

      // "INSERT INTO <table_name>"
      if (mysql_stmt_execute(stmt_ii))
        {
          print_stmt_error(stmt_ii, "Could not execute stmt for inserting into <table_name>");
          exit(1);
        }
    }
          
  mysql_stmt_free_result(stmt_s);
  if (mysql_stmt_close(stmt_s))
    {
      print_stmt_error(stmt_s, "Could not close stmt for selecting obj_id");
      exit(1);
    }
  if (mysql_stmt_close(stmt_i))
    {
      print_stmt_error(stmt_i, "Could not close stmt for inserting into xalt_object");
      exit(1);
    }
  if (mysql_stmt_close(stmt_ii))
    {
      print_stmt_error(stmt_ii, "Could not close stmt for inserting into <table_name>");
      exit(1);
    }
}

// This routine opens the socket connection to the db.  It tries 100 times and waits i*50 milliseconds
// before quiting.

MYSQL* xalt_open_mysql_connection(MYSQL* conn, ConfigParser& cp)
{
  int N    = 100;
  int msec = 50;

  for (int i = 1; i < N; ++i)
    {
      if (mysql_real_connect(conn, cp.host().c_str(), cp.user().c_str(), cp.passwd().c_str(), cp.db().c_str(), 3306, NULL, 0) != NULL)
        return conn;
      
      int milliseconds = i*msec;

      // convert milliseconds to ts and then sleep!
      struct timespec ts;
      ts.tv_sec = milliseconds / 1000;
      ts.tv_nsec = (milliseconds % 1000) * 1000000;
      nanosleep(&ts, NULL);
    }
  return conn;
}

