#include <time.h>
#include <my_global.h>
#include <mysql.h>
#include <string>
#include <stdlib.h>

#include "run_submission.h"
#include "ConfigParser.h"
#define  DATESZ 100

static int issueWarning = 1;

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

int select_run_id(MYSQL* conn, std::string& run_uuid, unsigned int* run_id)
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

  // STRING PARAM run_uuid;
  std::string::size_type len_uuid = run_uuid.size();
  param[0].buffer_type   = MYSQL_TYPE_STRING;
  param[0].buffer        = run_uuid.c_str();
  param[0].buffer_length = run_uuid.capacity();
  param[0].is_null       = 0;
  param[0].length        = &len_uuid;

  if (mysql_stmt_bind_param(stmt, param))
    {
      print_stmt_error(stmt, "Could not bind paramaters for selecting run_id");
      exit(1);
    }
      
  // UNSIGNED INT RESULT run_id;
  unsigned int id;
  result[0].buffer_type   = MYSQL_TYPE_LONG;
  result[0].buffer        = (void *) run_id;
  result[0].is_unsigned   = 1;
  result[0].is_null       = 0;
  result[0].length        = 0;

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
  int iret = mysql_stmt_fetch(stmt);

  if (mysql_stmt_close(stmt))
    {
      print_stmt_error(stmt, "Could not close stmt for selecting run_id");
      exit(1);
    }

  return iret;
}

void insert_xalt_run_record(MYSQL* conn, table& userT, std::string& usr_cmdline, int* run_id)
{
  time_t sTime = (time_t) strtod(userT["start_time"].c_str(), NULL);
  char dateStr[DATESZ];
  strftime(dateStr, DATESZ, "%c", localtime(&stime));

  
  


}

void insert_objects(MYSQL* conn, const char* table_name, int run_id, std::vector<Libpair>& lddA, std::string& syshost,
                    ReverseMapT& reverseMapT)
{
}

void insert_envT(MYSQL* conn, int run_id, table& envT)
{
}

void update_xalt_run_record(MYSQL* conn, int run_id, table& userT)
{
  double end_time = strtod(userT["end_time"],NULL);
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

  
  double run_time = strtod(userT["run_time"],NULL);

  MYSQL_BIND param[3];
  memset((void *) param,  0, sizeof(param));

  param[0].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[0].buffer        = (void *) &run_time;
  param[0].is_unsigned   = 0;
  param[0].is_null       = 0;
  param[0].length        = 0;

  param[1].buffer_type   = MYSQL_TYPE_DOUBLE;
  param[1].buffer        = (void *) &end_time;
  param[1].is_unsigned   = 0;
  param[1].is_null       = 0;
  param[1].length        = 0;

  param[2].buffer_type   = MYSQL_TYPE_LONG;
  param[2].buffer        = (void *) &run_id;
  param[2].is_unsigned   = 1;
  param[2].is_null       = 0;
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

void direct2db(std::string& confFn, std::string& usr_cmdline, table& envT, table& userT, table& recordT, std::vector<Libpair>& lddA)
{
  ConfigParser cp(confFn.c_str());

  MYSQL *conn = mysql_init(NULL);

  if (con == NULL)
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
  if (select_run_id(conn, run_uuid, &run_id))
    {
      // if here there was no previous record found.
      insert_xalt_run_record(conn, userT, usr_cmdline, &run_id);
      
      // Store objects
      insert_objects(conn, "join_run_object", run_id, lddA, userT['syshost'], reverseMapT);

      // Store environment
      insert_envT(conn, run_id, envT);
    }
  else
    // There was a previous record with this run_uuid, update end time (if positive)
    update_xalt_run_record(conn, run_id, userT);

  return;
}

