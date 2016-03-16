#include <my_global.h>
#include <mysql.h>
#include "run_submission.h"
#include "ConfigParser.h"

static int issueWarning = 1;

void finish_with_error(MYSQL *con)
{
  if (issueWarning)
    {
      fprintf(stderr, "RTM %s\n", mysql_error(con));
      mysql_close(con);
    }
  exit(1);        
}

void direct2db(std::string& confFn, std::string& usr_cmdline, table& envT, table& userT, table& recordT, std::vector<Libpair>& lddA)
{
  ConfigParser cp(confFn.c_str());

  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
    {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
    }  

  /* (1) Open DB */
  if (mysql_real_connect(con, cp.host().c_str(), cp.user().c_str(), cp.passwd().c_str(), cp.db().c_str(), 3306, NULL, 0) == NULL) 
    finish_with_error(con);
  

  std::string nameA[] = { "num_cores", "num_nodes", "account", "job_id", "queue", "submit_host" };
  int nameSz = sizeof(nameA)/sizeof(nameA[0]);

  for (int i = 0; i < nameSz; ++i)
    {
      std::cout << nameA[i] << "\n";
    }
  


  
}
