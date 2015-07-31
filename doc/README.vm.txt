The user name is xalt and the password for the account is xalt

The database is mysql, the root password is "xalt"
The xalt database user is "xaltuser" and the password is "xalt"

The steps to run and fill the database are:

**************************************************
Note that step 1 to 5 have already been done.
**************************************************

1) Create xaltuser account and password in MySQL:
   $ mysql -uroot -pxalt
   mysql> create user 'xaltuser'@'localhost' identified by 'xalt' ;
   mysql> grant all on xalt.* to 'xaltuser'@'localhost' ;
   mysql> quit;

2) Create xalt password file used with MySQL
    $ cd ~/use_xalt
    $ ./conf_create.py
	Database host: localhost
	Database user: xaltuser
	Database pass: xalt
	Database name: xalt

3) Initialize XALT DB:

    $ cd ~/use_xalt
    $ ./createDB.py

4) Create system executables:
    $ cd ~/t/xalt
    $ ./build_hello_executables.sh    (does an sudo, password is xalt)

5) Run some tests:
    $ cd ~/t/xalt
    $ ./run_hello_tests.sh

6) Load XALT db:

    $ cd ~/use_xalt
    $ ./load_xalt_db

7) Examine XALT db:
    $ mysql -uxaltuser -pxalt
    mysql> use xalt;
    mysql> select uuid,link_program,exec_path from xalt_link;
    mysql> select uuid,exec_path,module_name from xalt_run limit 5;
    mysql> select count(*),module_name from xalt_run group by module_name;
    mysql> select link_program,count(*) from xalt_link inner join xalt_run using (uuid) group by link_program;


