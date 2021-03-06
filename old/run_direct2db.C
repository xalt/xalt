#include <time.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "run_submission.h"
#include "run_direct2db.h"
#include "link_direct2db.h"
#include "ConfigParser.h"
#include "Json.h"
#include "xalt_utils.h"
#include "xalt_mysql_utils.h"
#define  DATESZ 100

#ifdef HAVE_MYSQL

typedef std::unordered_map<std::string, uint> TableIdx;

int select_run_id(MYSQL* conn, std::string& run_uuid, uint* run_id)
{
  const char* stmt_sql = "SELECT `run_id` FROM `xalt_run` WHERE `run_uuid` = ? limit 1";

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

  MYSQL_BIND param[1], result[1];
  memset((void *) param,  0, sizeof(param));
  memset((void *) result, 0, sizeof(result));

  // STRING PARAM[0] run_uuid;
  std::string::size_type len_run_uuid = run_uuid.size();
  param[0].buffer_type   = MYSQL_TYPE_STRING;
  param[0].buffer        = (void *) run_uuid.c_str();
  param[0].buffer_length = run_uuid.capacity();
  param[0].length        = &len_run_uuid;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for selecting run_id",__FILE__,__LINE__);
      exit(1);
    }
      
  // UNSIGNED INT RESULT[0] run_id;
  *run_id                 = 0;
  result[0].buffer_type   = MYSQL_TYPE_LONG;
  result[0].buffer        = (void *) run_id;
  result[0].is_unsigned   = 1;

  if (mysql_stmt_bind_result(stmt, result))
    {
      print_stmt_error(stmt, "Could not bind paramaters for selecting run_id",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for selecting run_id",__FILE__,__LINE__);
      exit(1);
    }

  // This function sets run_id if it is successful. It also sets iret to zero if it finds run_id;
  int iret = mysql_stmt_fetch(stmt); 

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for selecting run_id",__FILE__,__LINE__);
      exit(1);
    }

  // Clean up!
  mysql_stmt_free_result(stmt);
  return iret;
}

void insert_xalt_run_record(MYSQL* conn, Table& rmapT, Table& userT, DTable& userDT, Table& recordT,
                            std::string& usr_cmdline, std::string& hash_id, unsigned int* run_id)
{

  int         j=-1;
  const char* stmt_sql = "INSERT INTO xalt_run VALUES (NULL, ?,?,?, ?,?,?, ?,?,?, ?,?,?, ?,?,?, ?,?,?, ?,?,?, ?,COMPRESS(?))";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt for INSERT INTO xalt_run",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND param[23];
  memset((void *) param,  0, sizeof(param));

  // STRING PARAM[0] job_id
  j++;
  std::string& job_id    = userT["job_id"];
  std::string::size_type len_job_id = job_id.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) job_id.c_str();
  param[j].buffer_length = job_id.capacity();
  param[j].length        = &len_job_id;
    
  // STRING PARAM[1] run_uuid
  j++;
  std::string& run_uuid  = userT["run_uuid"];
  std::string::size_type len_run_uuid = run_uuid.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) run_uuid.c_str();
  param[j].buffer_length = run_uuid.capacity();
  param[j].length        = &len_run_uuid;
  
  // DATETIME PARAM[2] date 
  j++;
  MYSQL_TIME my_datetime;
  double     sTimeD       = userDT["start_time"];
  time_t     sTimeI       = (time_t) sTimeD;
  struct tm* sTime        = localtime(&sTimeI);
  my_datetime.year        = sTime->tm_year + 1900;
  my_datetime.month       = sTime->tm_mon  + 1;
  my_datetime.day         = sTime->tm_mday;
  my_datetime.hour        = sTime->tm_hour;
  my_datetime.minute      = sTime->tm_min;
  my_datetime.second      = sTime->tm_sec;
  my_datetime.second_part = 0;
  param[j].buffer_type    = MYSQL_TYPE_DATETIME;
  param[j].buffer         = &my_datetime;
  
  // STRING PARAM[3] syshost
  j++;
  std::string& syshost   = userT["syshost"];
  std::string::size_type len_syshost = syshost.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) syshost.c_str();
  param[j].buffer_length = syshost.capacity();
  param[j].length        = &len_syshost;
  
  // STRING PARAM[4] build_uuid (it can be NULL!)
  j++;
  my_bool                build_uuid_null_flag;
  std::string::size_type len_build_uuid = 0;
  std::string            build_uuid;
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  if (recordT.count("Build.UUID") > 0 )
    {
      build_uuid_null_flag = 0;
      build_uuid           = recordT["Build.UUID"];
      len_build_uuid       = build_uuid.size();
    }
  else if (recordT.count("Build_UUID") > 0 )
    {
      build_uuid_null_flag = 0;
      build_uuid           = recordT["Build_UUID"];
      len_build_uuid       = build_uuid.size();
    }
  else
    build_uuid_null_flag   = 1;

  param[j].buffer        = (void *) build_uuid.c_str();
  param[j].buffer_length = build_uuid.capacity();
  param[j].is_null       = &build_uuid_null_flag;
  param[j].length        = &len_build_uuid;
  
  // STRING PARAM[5] hash_id (sha1sum of executable)
  j++;
  std::string::size_type len_hash_id = hash_id.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) hash_id.c_str();
  param[j].buffer_length = hash_id.capacity();
  param[j].length        = &len_hash_id;

  // STRING PARAM[6] account
  j++;
  std::string& account   = userT["account"];
  std::string::size_type len_account = account.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) account.c_str();
  param[j].buffer_length = account.capacity();
  param[j].length        = &len_account;

  // STRING PARAM[7] exec_type
  j++;
  std::string& exec_type = userT["exec_type"];
  std::string::size_type len_exec_type = exec_type.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) exec_type.c_str();
  param[j].buffer_length = exec_type.capacity();
  param[j].length        = &len_exec_type;

  // DOUBLE PARAM[8] start_time
  j++;
  param[j].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[j].buffer        = (void *) &sTimeD;

  // DOUBLE PARAM[9] end_time
  j++;
  double eTimeD          = userDT["end_time"];
  param[j].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[j].buffer        = (void *) &eTimeD;

  // DOUBLE PARAM[10] run_time
  j++;
  double rTimeD        = userDT["run_time"];
  param[j].buffer_type = MYSQL_TYPE_DOUBLE;
  param[j].buffer      = (void *) &rTimeD;

  // DOUBLE PARAM[11] probability
  j++;
  double probD         = userDT["probability"];
  param[j].buffer_type = MYSQL_TYPE_DOUBLE;
  param[j].buffer      = (void *) &probD;

  // INT PARAM[12] num_cores
  j++;
  uint num_cores       = (uint) userDT["num_cores"];
  param[j].buffer_type = MYSQL_TYPE_LONG;
  param[j].buffer      = (void *) &num_cores;
  param[j].is_unsigned = 1;

  // INT PARAM[13] num_nodes
  j++;
  uint num_nodes       = (uint) userDT["num_nodes"];
  param[j].buffer_type = MYSQL_TYPE_LONG;
  param[j].buffer      = (void *) &num_nodes;
  param[j].is_unsigned = 1;

  // SHORT PARAM[14] num_threads
  j++;
  unsigned short int num_threads = (unsigned short int) userDT["num_threads"];
  param[j].buffer_type = MYSQL_TYPE_SHORT;
  param[j].buffer      = (void *) &num_threads;
  param[j].is_unsigned = 1;

  // STRING PARAM[15] queue
  j++;
  std::string& queue     = userT["queue"];
  std::string::size_type len_queue = queue.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) queue.c_str();
  param[j].buffer_length = queue.capacity();
  param[j].length        = &len_queue;

  // Int PARAM[16] sum_runs
  j++;
  uint sum_runs        = 0;
  param[j].buffer_type = MYSQL_TYPE_LONG;
  param[j].buffer      = (void *) &sum_runs;
  param[j].is_unsigned = 1;
  
  // DOUBLE PARAM[17] sum_times
  j++;
  double sum_times     = 0.0;
  param[j].buffer_type = MYSQL_TYPE_DOUBLE;
  param[j].buffer      = (void *) &sum_times;


  // STRING PARAM[18] user
  j++;
  std::string& user      = userT["user"];
  std::string::size_type len_user = user.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) user.c_str();
  param[j].buffer_length = user.capacity();
  param[j].length        = &len_user;

  // STRING PARAM[19] exec_path
  j++;
  std::string& exec_path  = userT["exec_path"];
  std::string::size_type len_exec_path = exec_path.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) exec_path.c_str();
  param[j].buffer_length = exec_path.capacity();
  param[j].length        = &len_exec_path;

  // STRING PARAM[20] module_name
  j++;
  const int              module_name_sz = 64;
  char                   module_name[module_name_sz];
  std::string::size_type len_module_name = 0;
  my_bool                module_name_null_flag = 0;
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) &module_name[0];
  param[j].buffer_length = module_name_sz;
  param[j].is_null       = &module_name_null_flag;
  param[j].length        = &len_module_name;
  if (path2module(exec_path.c_str(), rmapT, module_name,module_name_sz))
    {
      module_name_null_flag = 0;
      len_module_name       = strlen(module_name);
    }
  else
    module_name_null_flag = 1;

  // STRING PARAM[21] cwd
  j++;
  std::string& cwd       = userT["cwd"];
  std::string::size_type len_cwd = cwd.size();
  param[j].buffer_type   = MYSQL_TYPE_STRING;
  param[j].buffer        = (void *) cwd.c_str();
  param[j].buffer_length = cwd.capacity();
  param[j].length        = &len_cwd;

  // BLOB PARAM[22] cmdline
  j++;
  std::string::size_type len_cmdline = usr_cmdline.size();
  param[j].buffer_type   = MYSQL_TYPE_BLOB;
  param[j].buffer        = (void *) usr_cmdline.c_str();
  param[j].buffer_length = usr_cmdline.capacity();
  param[j].length        = &len_cmdline;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for insert xalt_run",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for insert xalt_run",__FILE__,__LINE__);
      exit(1);
    }

  *run_id = (unsigned int) mysql_stmt_insert_id(stmt);

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for insert xalt_run",__FILE__,__LINE__);
      exit(1);
    }
}


void buildEnvNameT(MYSQL* conn, TableIdx& envNameT)
{
  const char* stmt_sql = "SELECT `env_id`, `env_name` FROM `xalt_env_name`";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt for selecting env_id",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND result[2];
  memset((void *) result,  0, sizeof(result));

  //UNSIGNED INT RESULT[0] env_id;
  uint env_id = 0;
  my_bool                   env_id_null_flag = 0;
  result[0].buffer_type   = MYSQL_TYPE_LONG;
  result[0].buffer        = (void *) &env_id;
  result[0].is_unsigned   = 1;
  result[0].is_null       = &env_id_null_flag;
  
  // (C)STRING RESULT[1] env_name
  const size_t env_name_sz = 64;
  char env_name[env_name_sz+1];

  size_t len_env_name;
  my_bool env_name_null_flag = 0;
  result[1].buffer_type   = MYSQL_TYPE_STRING;
  result[1].buffer        = (void *) &env_name[0];
  result[1].buffer_length = env_name_sz;
  result[1].length        = &len_env_name;
  result[1].is_null       = &env_name_null_flag;

  if (mysql_stmt_bind_result(stmt, result))
    {
      print_stmt_error(stmt, "Could not bind paramaters for selecting env_id",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for selecting env_id",__FILE__,__LINE__);
      exit(1);
    }

  int iret; 
  while ((iret = mysql_stmt_fetch(stmt)) == 0)
    envNameT[env_name] = env_id;

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for selecting run_id",__FILE__,__LINE__);
      exit(1);
    }

  mysql_stmt_free_result(stmt);
}

uint findEnvNameIdx(MYSQL* conn, const std::string& env_name, TableIdx& envNameT)
{
  uint env_id = 0;
  TableIdx::const_iterator got = envNameT.find(env_name);
  if (got != envNameT.end())
    {
      env_id = got->second;      // return env_id
      return env_id;
    }

  
  const char* stmt_sql_i = "INSERT INTO xalt_env_name VALUES (NULL,?)";
  MYSQL_STMT *stmt_i = mysql_stmt_init(conn);
  if (!stmt_i)
    {
      print_stmt_error(stmt_i, "mysql_stmt_init(), out of memmory(2)",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_i, stmt_sql_i, strlen(stmt_sql_i)))
    {
      print_stmt_error(stmt_i, "Could not prepare stmt_i for insert into xalt_env_name",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND param_i[1];
  memset((void *) param_i,  0, sizeof(param_i));

  // STRING PARAM_I[0] env_name
  std::string::size_type     len_env_name = env_name.size();
  param_i[0].buffer_type   = MYSQL_TYPE_STRING;
  param_i[0].buffer        = (void *) env_name.c_str();
  param_i[0].buffer_length = env_name.capacity();
  param_i[0].length        = &len_env_name;
  
  if (mysql_stmt_bind_param(stmt_i, param_i))
    {
      print_stmt_error(stmt_i, "Could not bind paramaters for inserting into xalt_env_name",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_execute(stmt_i) == 0)
    env_id = (uint) mysql_stmt_insert_id(stmt_i);

  // Clean up stmt_i
  if (mysql_stmt_close(stmt_i))
    {
      print_stmt_error(stmt_i, "Could not close stmt for insert xalt_env_name",__FILE__,__LINE__);
      exit(1);
    }
  
  if (env_id > 0)
    {
      envNameT[env_name] = env_id;
      return env_id;
    }
  
  // If here then the insert failed because another process has already assigned
  // env_name to an env_id;  So go find it with a SELECT
  const char* stmt_sql_s = "SELECT env_id FROM xalt_env_name WHERE env_name=? limit 1";

  MYSQL_STMT *stmt_s = mysql_stmt_init(conn);
  if (!stmt_s)
    {
      print_stmt_error(stmt_s, "mysql_stmt_init(), out of memmory",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_s, stmt_sql_s, strlen(stmt_sql_s)))
    {
      print_stmt_error(stmt_s, "Could not prepare stmt_s for select obj_id",__FILE__,__LINE__);
      exit(1);
    }

  MYSQL_BIND param_s[1], result_s[1];
  memset((void *) param_s,  0, sizeof(param_s));
  memset((void *) result_s, 0, sizeof(result_s));

  // STRING PARAM_S[0] env_name
  param_s[0].buffer_type   = MYSQL_TYPE_STRING;
  param_s[0].buffer        = (void *) env_name.c_str();
  param_s[0].buffer_length = env_name.capacity();
  param_s[0].length        = &len_env_name;

  if (mysql_stmt_bind_param(stmt_s, param_s))
    {
      print_stmt_error(stmt_s, "Could not bind paramaters for selecting env_id(1)",__FILE__,__LINE__);
      exit(1);
    }

  // UNSIGNED INT RESULT_S env_id;
  result_s[0].buffer_type   = MYSQL_TYPE_LONG;
  result_s[0].buffer        = (void *) &env_id;
  result_s[0].is_unsigned   = 1;
  result_s[0].length        = 0;

  if (mysql_stmt_bind_result(stmt_s, result_s))
    {
      print_stmt_error(stmt_s, "Could not bind paramaters for selecting env_id(2)",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_execute(stmt_s))
    {
      print_stmt_error(stmt_s, "Could not execute stmt for selecting env_id",__FILE__,__LINE__);
      exit(1);
    }
  mysql_stmt_free_result(stmt_s);

  envNameT[env_name] = env_id;
  return env_id;
}

void insert_envT(MYSQL* conn, uint run_id, time_t epoch, Table& envT)
{
  Json json;
  json.add(NULL,envT);
  json.fini();
  std::string jsonStr = json.result();

  const char* stmt_sql = "INSERT INTO xalt_total_env VALUES (NULL, ?, ?, COMPRESS(?))";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt_s for INSERT INTO xalt_total_env",__FILE__,__LINE__);
      exit(1);
    }
  
  MYSQL_BIND param[3];
  memset((void *) param,  0, sizeof(param));

  // INT PARAM[0] run_id
  param[0].buffer_type = MYSQL_TYPE_LONG;
  param[0].buffer      = (void *) &run_id;
  param[0].is_unsigned = 1;

  // DATE PARAM[1] date
  MYSQL_TIME my_datetime;
  struct tm* curr_time    = localtime(&epoch);
  my_datetime.year        = curr_time->tm_year + 1900;
  my_datetime.month       = curr_time->tm_mon  + 1;
  my_datetime.day         = curr_time->tm_mday;
  my_datetime.hour        = 0;
  my_datetime.minute      = 0;
  my_datetime.second      = 0;
  my_datetime.second_part = 0;
  param[1].buffer_type    = MYSQL_TYPE_DATE;
  param[1].buffer         = &my_datetime;

  // STRING PARAM[1] jsonStr
  std::string::size_type   len_jsonStr = jsonStr.size();
  param[2].buffer_type   = MYSQL_TYPE_STRING;
  param[2].buffer        = (void *) jsonStr.c_str();
  param[2].buffer_length = jsonStr.capacity();
  param[2].length        = &len_jsonStr;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for insert xalt_total_env",__FILE__,__LINE__);
      exit(1);
    }
  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for insert xalt_total_env",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for insert xalt_total_env",__FILE__,__LINE__);
      exit(1);
    }
}

void insert_filtered_envT(MYSQL* conn, uint run_id, time_t epoch, Table& envT)
{


  TableIdx envNameT;
  buildEnvNameT(conn, envNameT);

  const char* stmt_sql = "INSERT INTO join_run_env VALUES (NULL, ?,?,?,?)";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt_s for INSERT INTO join_run_env",__FILE__,__LINE__);
      exit(1);
    }
  
  MYSQL_BIND param[4];
  memset((void *) param,  0, sizeof(param));

  // INT PARAM[0] env_id
  uint env_id;
  param[0].buffer_type = MYSQL_TYPE_LONG;
  param[0].buffer      = (void *) &env_id;
  param[0].is_unsigned = 1;

  // INT PARAM[1] run_id
  param[1].buffer_type = MYSQL_TYPE_LONG;
  param[1].buffer      = (void *) &run_id;
  param[1].is_unsigned = 1;

  // DATE PARAM[2] date
  MYSQL_TIME my_datetime;
  struct tm* curr_time    = localtime(&epoch);
  my_datetime.year        = curr_time->tm_year + 1900;
  my_datetime.month       = curr_time->tm_mon  + 1;
  my_datetime.day         = curr_time->tm_mday;
  my_datetime.hour        = 0;
  my_datetime.minute      = 0;
  my_datetime.second      = 0;
  my_datetime.second_part = 0;
  param[2].buffer_type  = MYSQL_TYPE_DATE;
  param[2].buffer       = &my_datetime;

  // STRING PARAM[3] env_value
  const int env_value_sz = 65536;
  char      env_value[env_value_sz];
  std::string::size_type   len_env_value = 0;   // set length in loop.
  param[3].buffer_type   = MYSQL_TYPE_STRING;
  param[3].buffer        = (void *) &env_value[0];
  param[3].buffer_length = env_value_sz;
  param[3].length        = &len_env_value;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for insert join_run_env",__FILE__,__LINE__);
      exit(1);
    }
  
  for (auto const & it : envT)
    {
      const std::string& env_name = it.first;
      env_id                      = findEnvNameIdx(conn, env_name, envNameT);
      len_env_value               = it.second.size();
      strcpy(env_value, it.second.c_str());

      // INSERT INTO join_run_env
      if (mysql_stmt_execute(stmt))
        {
          char istr[12];
          sprintf(&istr[0], "%d", env_id);

          std::string str = "Could not execute stmt for insert join_run_env: name: \"";
          str.append(env_name);
          str.append("\", value: \"");
          str.append(env_value);
          str.append("\" env_id: ");
          str.append(istr);
          
          print_stmt_error(stmt, str.c_str() ,__FILE__,__LINE__);
          exit(1);
        }
    }

  // clean up.
  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for insert join_run_env",__FILE__,__LINE__);
      exit(1);
    }
}

void update_xalt_run_record(MYSQL* conn, uint run_id, DTable& userDT)
{
  double end_time = userDT["end_time"];
  if (end_time <= 0.0)
    return;
  
  const char* stmt_sql = "UPDATE xalt_run SET run_time = ?, end_time = ? WHERE run_id = ?";
  MYSQL_STMT* stmt     = mysql_stmt_init(conn);
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

  
  double run_time = userDT["run_time"];

  MYSQL_BIND param[3];
  memset((void *) param,  0, sizeof(param));

  // DOUBLE PARAM[0] run_time
  param[0].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[0].buffer        = (void *) &run_time;
  param[0].is_unsigned   = 0;
  param[0].length        = 0;

  // DOUBLE PARAM[1] end_time
  param[1].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[1].buffer        = (void *) &end_time;
  param[1].is_unsigned   = 0;
  param[1].length        = 0;

  // UINT PARAM[2] run_id
  param[2].buffer_type   = MYSQL_TYPE_LONG;
  param[2].buffer        = (void *) &run_id;
  param[2].is_unsigned   = 1;
  param[2].length        = 0;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for update run_time",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for update run_time",__FILE__,__LINE__);
      exit(1);
    }

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for update run_time",__FILE__,__LINE__);
      exit(1);
    }
}

void run_direct2db(const char* confFn, std::string& usr_cmdline, std::string& hash_id, Table& rmapT,
                   Table& envT, Table& userT, DTable& userDT, Table& recordT, std::vector<Libpair>& lddA)
{
  ConfigParser cp(confFn);

  MYSQL *conn = mysql_init(NULL);

  if (conn == NULL)
    {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
    }  

  // Note the interface is in autocommit mode.  This should probably change to transaction for the entire run record.

  /* (1) Open DB */
  if (xalt_open_mysql_connection(conn, cp) == NULL)
    finish_with_error(conn);
  
  mysql_query(conn,"START TRANSACTION");

  unsigned int run_id;
  if (select_run_id(conn, userT["run_uuid"], &run_id)) // select_run_id returns a non-zero if [[run_id]] is not found.
    {
      time_t startTime = userDT["start_time"];

      // if here there was no previous record found (and run_id is unset!).
      insert_xalt_run_record(conn, rmapT, userT, userDT, recordT, usr_cmdline, hash_id, &run_id);
      
      // Store objects
      insert_objects(conn, "join_run_object", startTime, run_id, lddA, userT["syshost"], rmapT);

      // Store environment
      // There is not enough room to save total env table.
      // insert_envT(         conn, run_id, startTime, envT);
      insert_filtered_envT(conn, run_id, startTime, envT);
    }
  else
    {
      // If the end record is loaded first then the start record is used to store shared libraries
      if (userDT["end_time"] < 1.0)
        {
          time_t startTime = userDT["start_time"];
          insert_objects(conn, "join_run_object", startTime, run_id, lddA, userT["syshost"], rmapT);
        }
      

      // There was a previous record with this run_uuid, update end time (if positive)
      update_xalt_run_record(conn, run_id, userDT) ;
    }

  mysql_query(conn,"COMMIT");
  mysql_close(conn);
  return;
}

#else
void run_direct2db(const char* confFn, std::string& usr_cmdline, std::string& hash_id, Table& rmapT,
                   Table& envT, Table& userT, DTable& userDT, Table& recordT, std::vector<Libpair>& lddA)
{
  fprintf(stderr,"This version of XALT was not built with MySQL support.\n"
          "You can not use the direct2db transmission style.  Aborting!\n");
  exit(1);
}

#endif //HAVE_MYSQL
