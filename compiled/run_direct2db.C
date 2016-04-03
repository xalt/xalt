#include <time.h>
#include <my_global.h>
#include <mysql.h>
#undef min
#undef max
#undef test
#include <string>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "run_submission.h"
#include "ConfigParser.h"
#include "xalt_regex.h"
#include "Json.h"
#include "xalt_utils.h"
#include "xalt_mysql_utils.h"
#define  DATESZ 100

typedef std::unordered_map<std::string, uint> TableIdx;


int select_run_id(MYSQL* conn, std::string& run_uuid, uint* run_id)
{
  const char* stmt_sql = "SELECT `run_id` FROM `xalt_run` WHERE `run_uuid` = ? limit 1";

  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt for selecting run_id");
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
      print_stmt_error(stmt, "Could not bind paramaters for selecting run_id");
      exit(1);
    }
      
  // UNSIGNED INT RESULT[0] run_id;
  *run_id                 = 0;
  result[0].buffer_type   = MYSQL_TYPE_LONG;
  result[0].buffer        = (void *) run_id;
  result[0].is_unsigned   = 1;

  if (mysql_stmt_bind_result(stmt, result))
    {
      print_stmt_error(stmt, "Could not bind paramaters for selecting run_id");
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for selecting run_id");
      exit(1);
    }

  // This function sets run_id if it is successful. It also sets iret to zero if it finds run_id;
  int iret = mysql_stmt_fetch(stmt); 

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for selecting run_id");
      exit(1);
    }

  // Clean up!
  mysql_stmt_free_result(stmt);
  return iret;
}

void insert_xalt_run_record(MYSQL* conn, Table& rmapT, Table& userT, Table& recordT, std::string& usr_cmdline,
                            std::string& hash_id, unsigned int* run_id)
{

  const char* stmt_sql = "INSERT INTO xalt_run VALUES (NULL, ?,?,?, ?,?,?, ?,?,?, ?,?,?, ?,?,?, ?,?,?, ?,?,?, COMPRESS(?))";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt for selecting run_id");
      exit(1);
    }

  MYSQL_BIND param[22];
  memset((void *) param,  0, sizeof(param));

  // STRING PARAM[0] job_id
  std::string& job_id    = userT["job_id"];
  std::string::size_type len_job_id = job_id.size();
  param[0].buffer_type   = MYSQL_TYPE_STRING;
  param[0].buffer        = (void *) job_id.c_str();
  param[0].buffer_length = job_id.capacity();
  param[0].length        = &len_job_id;
    
  // STRING PARAM[1] run_uuid
  std::string& run_uuid  = userT["run_uuid"];
  std::string::size_type len_run_uuid = run_uuid.size();
  param[1].buffer_type   = MYSQL_TYPE_STRING;
  param[1].buffer        = (void *) run_uuid.c_str();
  param[1].buffer_length = run_uuid.capacity();
  param[1].length        = &len_run_uuid;
  
  // DATETIME PARAM[2] date 
  MYSQL_TIME my_datetime;
  double     sTimeD       = strtod(userT["start_time"].c_str(), NULL);
  time_t     sTimeI       = (time_t) sTimeD;
  struct tm* sTime        = localtime(&sTimeI);
  my_datetime.year        = sTime->tm_year + 1900;
  my_datetime.month       = sTime->tm_mon  + 1;
  my_datetime.day         = sTime->tm_mday;
  my_datetime.hour        = sTime->tm_hour;
  my_datetime.minute      = sTime->tm_min;
  my_datetime.second      = sTime->tm_sec;
  my_datetime.second_part = 0;
  param[2].buffer_type    = MYSQL_TYPE_DATETIME;
  param[2].buffer         = &my_datetime;
  
  // STRING PARAM[3] syshost
  std::string& syshost   = userT["syshost"];
  std::string::size_type len_syshost = syshost.size();
  param[3].buffer_type   = MYSQL_TYPE_STRING;
  param[3].buffer        = (void *) syshost.c_str();
  param[3].buffer_length = syshost.capacity();
  param[3].length        = &len_syshost;
  
  // STRING PARAM[4] build_uuid (it can be NULL!)
  my_bool                build_uuid_null_flag;
  std::string::size_type len_build_uuid = 0;
  std::string            build_uuid;
  param[4].buffer_type    = MYSQL_TYPE_STRING;
  if (recordT.count("Build.UUID") == 0)
    build_uuid_null_flag   = 1;
  else
    {
      build_uuid_null_flag = 0;
      build_uuid           = recordT["Build.UUID"];
      len_build_uuid       = build_uuid.size();
    }
  param[4].buffer        = (void *) build_uuid.c_str();
  param[4].buffer_length = build_uuid.capacity();
  param[4].is_null       = &build_uuid_null_flag;
  param[4].length        = &len_build_uuid;
  
  // STRING PARAM[5] hash_id (sha1sum of executable)
  std::string::size_type len_hash_id = hash_id.size();
  param[5].buffer_type   = MYSQL_TYPE_STRING;
  param[5].buffer        = (void *) hash_id.c_str();
  param[5].buffer_length = hash_id.capacity();
  param[5].length        = &len_hash_id;

  // STRING PARAM[6] account
  std::string& account   = userT["account"];
  std::string::size_type len_account = account.size();
  param[6].buffer_type   = MYSQL_TYPE_STRING;
  param[6].buffer        = (void *) account.c_str();
  param[6].buffer_length = account.capacity();
  param[6].length        = &len_account;

  // STRING PARAM[7] exec_type
  std::string& exec_type = userT["exec_type"];
  std::string::size_type len_exec_type = exec_type.size();
  param[7].buffer_type   = MYSQL_TYPE_STRING;
  param[7].buffer        = (void *) exec_type.c_str();
  param[7].buffer_length = exec_type.capacity();
  param[7].length        = &len_exec_type;

  // DOUBLE PARAM[8] start_time
  param[8].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[8].buffer        = (void *) &sTimeD;

  // DOUBLE PARAM[9] end_time
  double eTimeD          = strtod(userT["end_time"].c_str(), NULL);
  param[9].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[9].buffer        = (void *) &eTimeD;

  // DOUBLE PARAM[10] run_time
  double rTimeD         = strtod(userT["run_time"].c_str(), NULL);
  param[10].buffer_type = MYSQL_TYPE_DOUBLE;
  param[10].buffer      = (void *) &rTimeD;

  // INT PARAM[11] num_cores
  uint num_cores         = (uint) strtol(userT["num_cores"].c_str(), NULL, 10);
  param[11].buffer_type = MYSQL_TYPE_LONG;
  param[11].buffer      = (void *) &num_cores;
  param[11].is_unsigned = 1;

  // INT PARAM[12] job_num_cores  // think about removing this!
  param[12].buffer_type = MYSQL_TYPE_LONG;
  param[12].buffer      = (void *) &num_cores;
  param[12].is_unsigned = 1;

  // INT PARAM[13] num_nodes
  int num_nodes         = (int) strtol(userT["num_nodes"].c_str(), NULL, 10);
  param[13].buffer_type = MYSQL_TYPE_LONG;
  param[13].buffer      = (void *) &num_nodes;
  param[13].is_unsigned = 1;

  // SHORT PARAM[14] num_threads
  short int num_threads = (short int) strtol(userT["num_threads"].c_str(), NULL, 10);
  param[14].buffer_type = MYSQL_TYPE_SHORT;
  param[14].buffer      = (void *) &num_threads;
  param[14].is_unsigned = 1;

  // STRING PARAM[15] queue
  std::string& queue      = userT["queue"];
  std::string::size_type len_queue = queue.size();
  param[15].buffer_type   = MYSQL_TYPE_STRING;
  param[15].buffer        = (void *) queue.c_str();
  param[15].buffer_length = queue.capacity();
  param[15].length        = &len_account;

  // SHORT PARAM[16] exit_code // Think about removing this!
  short int exit_code   = 0;
  param[16].buffer_type = MYSQL_TYPE_SHORT;
  param[16].buffer      = (void *) &exit_code;

  // STRING PARAM[17] user
  std::string& user       = userT["user"];
  std::string::size_type len_user = user.size();
  param[17].buffer_type   = MYSQL_TYPE_STRING;
  param[17].buffer        = (void *) user.c_str();
  param[17].buffer_length = user.capacity();
  param[17].length        = &len_user;

  // STRING PARAM[18] exec_path
  std::string& exec_path  = userT["exec_path"];
  std::string::size_type len_exec_path = exec_path.size();
  param[18].buffer_type   = MYSQL_TYPE_STRING;
  param[18].buffer        = (void *) exec_path.c_str();
  param[18].buffer_length = exec_path.capacity();
  param[18].length        = &len_exec_path;

  // STRING PARAM[19] module_name
  const int              module_name_sz = 64;
  char                   module_name[module_name_sz];
  std::string::size_type len_module_name = 0;
  my_bool                module_name_null_flag = 0;
  param[19].buffer_type   = MYSQL_TYPE_STRING;
  param[19].buffer        = (void *) &module_name[0];
  param[19].buffer_length = module_name_sz;
  param[19].is_null       = &module_name_null_flag;
  param[19].length        = &len_module_name;
  if (path2module(exec_path.c_str(), rmapT, module_name,module_name_sz))
    {
      module_name_null_flag = 0;
      len_module_name       = strlen(module_name);
    }
  else
    module_name_null_flag = 1;

  // STRING PARAM[20] cwd
  std::string& cwd        = userT["cwd"];
  std::string::size_type len_cwd = cwd.size();
  param[20].buffer_type   = MYSQL_TYPE_STRING;
  param[20].buffer        = (void *) cwd.c_str();
  param[20].buffer_length = cwd.capacity();
  param[20].length        = &len_cwd;

  // BLOB PARAM[21] cmdline
  std::string::size_type len_cmdline = usr_cmdline.size();
  param[21].buffer_type   = MYSQL_TYPE_BLOB;
  param[21].buffer        = (void *) usr_cmdline.c_str();
  param[21].buffer_length = usr_cmdline.capacity();
  param[21].length        = &len_cmdline;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for insert xalt_run");
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for insert xalt_run");
      exit(1);
    }

  *run_id = (unsigned int) mysql_stmt_insert_id(stmt);

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for insert xalt_run");
      exit(1);
    }
}


void buildEnvNameT(MYSQL* conn, TableIdx& envNameT)
{
  const char* stmt_sql = "SELECT `env_id`, `env_name` FROM `xalt_env_name`";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt for selecting run_id");
      exit(1);
    }

  MYSQL_BIND result[2];
  memset((void *) result,  0, sizeof(result));

  //UNSIGNED INT RESULT[0] env_id;
  uint env_id;
  result[0].buffer_type   = MYSQL_TYPE_LONG;
  result[0].buffer        = (void *) &env_id;
  result[0].is_unsigned   = 1;
  
  // (C)STRING RESULT[1] env_name
  const size_t env_name_sz = 128;
  char env_name[env_name_sz+1];

  size_t len_env_name;
  result[1].buffer_type   = MYSQL_TYPE_STRING;
  result[1].buffer        = (void *) &env_name[0];
  result[1].buffer_length = len_env_name+1;
  result[1].length        = &len_env_name;

  if (mysql_stmt_bind_result(stmt, result))
    {
      print_stmt_error(stmt, "Could not bind paramaters for selecting run_id");
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for selecting run_id");
      exit(1);
    }

  while (mysql_stmt_fetch(stmt) == 0)
    envNameT[env_name] = env_id;

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for selecting run_id");
      exit(1);
    }

  mysql_stmt_free_result(stmt);
}

bool reject_env_name(const std::string& env_name)
{
  regex_t regex;
  char    msgbuf[100];
  bool    ret = (acceptEnvSz != 0);

  for (int i = 0; i < acceptEnvSz; ++i)
    {
      if (regcomp(&regex, acceptEnvA[i], 0))
        {
          fprintf(stderr,"Could not compile regex \"%s\"\n", acceptEnvA[i]);
          exit(1);
        }
      int iret = regexec(&regex, env_name.c_str(), 0, NULL, 0);
      if (iret == 0)
        {
          ret = false;
          break;
        }
      else if (iret != REG_NOMATCH)
        {
          regerror(iret, &regex, msgbuf, sizeof(msgbuf));
          fprintf(stderr, "acceptEnvA Regex match failed: %s\n", msgbuf);
          exit(1);
        }
      regfree(&regex);
    }

  if (ret)
    return true;
  
  for (int i = 0; i < ignoreEnvSz; ++i)
    {
      if (regcomp(&regex, ignoreEnvA[i], 0))
        {
          fprintf(stderr,"Could not compile regex \"%s\"\n", ignoreEnvA[i]);
          exit(1);
        }
      int iret = regexec(&regex, env_name.c_str(), 0, NULL, 0);
      if (iret == 0)
        return true;
      else if (iret != REG_NOMATCH)
        {
          regerror(iret, &regex, msgbuf, sizeof(msgbuf));
          fprintf(stderr, "ignoreEnvA Regex match failed: %s\n", msgbuf);
          exit(1);
        }
      regfree(&regex);
    }
  return false;
}  

uint findEnvNameIdx(MYSQL* conn, const std::string& env_name, TableIdx& envNameT)
{
  uint env_id = 0;
  TableIdx::const_iterator got = envNameT.find(env_name);
  if (got != envNameT.end())
    return got->second;      // return env_id

  
  const char* stmt_sql_i = "INSERT INTO xalt_env_name VALUES (NULL,?)";
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
      print_stmt_error(stmt_i, "Could not bind paramaters for inserting into xalt_object");
      exit(1);
    }

  if (mysql_stmt_execute(stmt_i) == 0)
    env_id = (uint) mysql_stmt_insert_id(stmt_i);

  // Clean up stmt_i
  if (mysql_stmt_close(stmt_i))
    {
      print_stmt_error(stmt_i, "Could not close stmt for insert xalt_run");
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
      print_stmt_error(stmt_s, "mysql_stmt_init(), out of memmory");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt_s, stmt_sql_s, strlen(stmt_sql_s)))
    {
      print_stmt_error(stmt_s, "Could not prepare stmt_s for select obj_id");
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
      print_stmt_error(stmt_s, "Could not bind paramaters for selecting env_id(1)");
      exit(1);
    }

  // UNSIGNED INT RESULT_S env_id;
  result_s[0].buffer_type   = MYSQL_TYPE_LONG;
  result_s[0].buffer        = (void *) &env_id;
  result_s[0].is_unsigned   = 1;
  result_s[0].length        = 0;

  if (mysql_stmt_bind_result(stmt_s, result_s))
    {
      print_stmt_error(stmt_s, "Could not bind paramaters for selecting env_id(2)");
      exit(1);
    }

  if (mysql_stmt_execute(stmt_s))
    {
      print_stmt_error(stmt_s, "Could not execute stmt for selecting env_id");
      exit(1);
    }
  mysql_stmt_free_result(stmt_s);

  envNameT[env_name] = env_id;
  return env_id;
}

void insert_envT(MYSQL* conn, uint run_id, Table& envT)
{
  Json json;
  json.add(NULL,envT);
  json.fini();
  std::string jsonStr = json.result();

  const char* stmt_sql = "INSERT INTO xalt_total_env VALUES (NULL, ?,COMPRESS(?))";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt_s for INSERT INTO xalt_total_env");
      exit(1);
    }
  
  MYSQL_BIND param[2];
  memset((void *) param,  0, sizeof(param));

  // INT PARAM[0] run_id
  param[0].buffer_type = MYSQL_TYPE_LONG;
  param[0].buffer      = (void *) &run_id;
  param[0].is_unsigned = 1;

  // STRING PARAM[1] jsonStr
  std::string::size_type   len_jsonStr = jsonStr.size();
  param[1].buffer_type   = MYSQL_TYPE_STRING;
  param[1].buffer        = (void *) jsonStr.c_str();
  param[1].buffer_length = jsonStr.capacity();
  param[1].length        = &len_jsonStr;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for insert xalt_total_env");
      exit(1);
    }
  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for insert xalt_total_env");
      exit(1);
    }

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for insert xalt_total_env");
      exit(1);
    }
}

void insert_filtered_envT(MYSQL* conn, uint run_id, Table& envT)
{


  TableIdx envNameT;
  buildEnvNameT(conn, envNameT);

  const char* stmt_sql = "INSERT INTO join_run_env VALUES (NULL, ?,?,?)";
  MYSQL_STMT *stmt = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt_s for INSERT INTO join_run_env");
      exit(1);
    }
  
  MYSQL_BIND param[3];
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

  // STRING PARAM[2] env_value
  const int env_value_sz = 65536;
  char      env_value[env_value_sz];

  std::string::size_type   len_env_value = 0;   // set length in loop.
  param[2].buffer_type   = MYSQL_TYPE_STRING;
  param[2].buffer        = (void *) &env_value[0];
  param[2].buffer_length = env_value_sz;
  param[2].length        = &len_env_value;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for insert join_run_env");
      exit(1);
    }
  
  for (auto it = envT.begin(); it != envT.end(); ++it)
    {
      const std::string& env_name  = it->first;
      if (reject_env_name(env_name)) continue;
      
      env_id        = findEnvNameIdx(conn, env_name, envNameT);
      len_env_value =   (*it).second.size();
      strcpy(env_value, (*it).second.c_str());

      // INSERT INTO join_run_env
      if (mysql_stmt_execute(stmt))
        {
          print_stmt_error(stmt, "Could not execute stmt for insert join_run_env");
          exit(1);
        }
    }

  // clean up.
  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for insert join_run_env");
      exit(1);
    }
}

void update_xalt_run_record(MYSQL* conn, uint run_id, Table& userT)
{
  double end_time = strtod(userT["end_time"].c_str(),NULL);
  if (end_time <= 0.0)
    return;
  
  const char* stmt_sql = "UPDATE xalt_run SET run_time = ?, end_time = ? WHERE run_id = ?";
  MYSQL_STMT* stmt     = mysql_stmt_init(conn);
  if (!stmt)
    {
      print_stmt_error(stmt, "mysql_stmt_init(), out of memmory");
      exit(1);
    }

  if (mysql_stmt_prepare(stmt, stmt_sql, strlen(stmt_sql)))
    {
      print_stmt_error(stmt, "Could not prepare stmt for selecting run_id");
      exit(1);
    }

  
  double run_time = strtod(userT["run_time"].c_str(),NULL);

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
      print_stmt_error(stmt, "Could not bind paramaters for update run_time");
      exit(1);
    }

  if (mysql_stmt_execute(stmt))
    {
      print_stmt_error(stmt, "Could not execute stmt for update run_time");
      exit(1);
    }

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for update run_time");
      exit(1);
    }
}

void run_direct2db(std::string& usr_cmdline, std::string& hash_id, Table& rmapT, Table& envT, Table& userT,
               Table& recordT, std::vector<Libpair>& lddA)
{
  ConfigParser cp("xalt_db.conf");

  MYSQL *conn = mysql_init(NULL);

  if (conn == NULL)
    {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
    }  

  // Note the interface is in autocommit mode.  This should probably change to transaction for the entire run record.

  /* (1) Open DB */
  if (mysql_real_connect(conn, cp.host().c_str(), cp.user().c_str(), cp.passwd().c_str(), cp.db().c_str(), 3306, NULL, 0) == NULL) 
    finish_with_error(conn);
  

  // Use this translate routine to extract values from the environment to provide standard value in the xalt_run table in DB;
  translate(envT, userT);
  
  unsigned int run_id;
  if (select_run_id(conn, userT["run_uuid"], &run_id)) // select_run_id return 0 if it found [[run_id]].
    {
      // if here there was no previous record found (and run_id is unset!).
      insert_xalt_run_record(conn, rmapT, userT, recordT, usr_cmdline, hash_id, &run_id);
      
      // Store objects
      insert_objects(conn, "join_run_object", run_id, lddA, userT["syshost"], rmapT);

      // Store environment
      insert_envT(         conn, run_id, envT);
      insert_filtered_envT(conn, run_id, envT);
    }
  else
    {
      // There was a previous record with this run_uuid, update end time (if positive)
      update_xalt_run_record(conn, run_id, userT);
    }

  mysql_close(conn);
  return;
}
