#include <iostream>
#include <pwd.h>
#include <sys/time.h>
#include <unordered_set>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>
#include <string.h>

#include "jsmn.h"
#include "f2db_Options.h"
#include "buildRmapT.h"
#include "epoch.h"
#include "xalt_utils.h"
#include "xalt_fgets_alloc.h"
#include "parseJsonStr.h"
#include "link_direct2db.h"
#include "run_direct2db.h"
#include "ProgressBar.h"

int buildUserTable(Table& users)
{
  char* xalt_user_str = getenv("XALT_USERS");
  if (xalt_user_str == NULL)
    return 0;

  std::string value;
  std::string userName;
  std::string homeDir;
  char*       start     = xalt_user_str;
  bool        done      = false;
  while(! done)
    {
      char * p = strchr(start,':');
      if (p)
        value.assign(start);
      else
        {
          done = true;
          value.assign(start, p - start);
        }
      size_t idx = value.find(";");
      if (idx == std::string::npos)
        {
          // find passwd record from systems:
          userName          = value;
          struct passwd* pw = getpwnam(userName.c_str());
          if (pw)
            users[userName] = pw->pw_dir; 
        }
      else
        {
          userName.assign(value,0,idx);    
          homeDir.assign(value, idx+1, std::string::npos);
          users[userName] = homeDir;
        }
      start = p+1;
    }
  return users.size();
}

int findFilesInDir(std::string& dir, const char* filePattern, Vstring& fileA)
{
  DIR* dirp = opendir(dir.c_str());
  if (dirp == NULL)
    return 0;

  struct dirent* dp;
  while( (dp = readdir(dirp)) != NULL)
    {
      if (fnmatch(filePattern, dp->d_name, 0) == 0)
        {
          std::string fullName(dir);
          fullName.append("/");
          fullName.append(dp->d_name);
          fileA.push_back(fullName);
       }
     }
  closedir(dirp);
  return fileA.size();
}

void removeFiles(Vstring& fileA)
{
  for (auto it = fileA.begin(); it < fileA.end(); ++it)
    unlink(it->c_str());
}

int link_json_fileA_to_db(f2db_Options& options, Table& rmapT, Vstring& fileA)
{
  jsmn_parser parser;
  jsmntok_t*  tokens;
  int         maxTokens = 1000;
  int         num       = 0;

  for (auto it = fileA.begin(); it < fileA.end(); ++it)
    {
      Set                  funcSet;
      std::vector<Libpair> libA;
      Vstring              linkLineA;
      Table                resultT;
      std::string          jsonStr;
      char*                buf = NULL;
      size_t               sz  = 0;

      FILE*  fp = fopen(it->c_str(),"r");
      if (fp == NULL)
        continue;
      if (options.reportFn())
        fprintf(stdout, "Processing link file: %s\n", it->c_str());

      while(xalt_fgets_alloc(fp, &buf, &sz))
        jsonStr.append(buf);
      free(buf);

      parseLinkJsonStr(it->c_str(), jsonStr, linkLineA, resultT, libA, funcSet);

      link_direct2db(options.confFn().c_str(), linkLineA, resultT, libA, funcSet, rmapT);
      num++;
    }
  return num;
}

int run_json_fileA_to_db(f2db_Options& options, Table& rmapT, Vstring& fileA)
{
  jsmn_parser parser;
  jsmntok_t*  tokens;
  int         maxTokens = 1000;
  int         num       = 0;

  for (auto it = fileA.begin(); it < fileA.end(); ++it)
    {
      std::string          usr_cmdline;
      std::string          hash_id;
      Table                envT;
      Table                userT;
      Table                recordT;
      std::vector<Libpair> libA;

      std::string          jsonStr;
      char*                buf = NULL;
      size_t               sz  = 0;

      FILE*  fp = fopen(it->c_str(),"r");
      if (fp == NULL)
        continue;
      if (options.reportFn())
        fprintf(stdout, "Processing run file: %s\n", it->c_str());
        
      while(xalt_fgets_alloc(fp, &buf, &sz))
        jsonStr.append(buf);
      free(buf);

      parseRunJsonStr(it->c_str(), jsonStr, usr_cmdline, hash_id, envT,
                      userT, recordT, libA);

      run_direct2db(options.confFn().c_str(), usr_cmdline, hash_id, rmapT, envT, userT, recordT, libA);
      num++;
    }
  return num;
}

int main(int argc, char* argv[])
{
  f2db_Options options(argc, argv);
  struct passwd* pw;
  
  double t1 = epoch();

  Table users;
  int   num           = buildUserTable(users);
  bool  haveUserTable = false;
  if (num == 0)
    {
      // Count the number of passwd entries:
      setpwent();
      while ( (pw = getpwent()) != NULL )
        num++;
      endpwent();
    }
  else
    haveUserTable = true;

  ProgressBar pbar(num);
  Vstring     xlibmapA;
  Table       rmapT;
  buildRmapT(rmapT, xlibmapA);

  int icnt   = 0;
  int iuser  = 0;
  int lnkCnt = 0;
  int runCnt = 0;

  std::string xaltDir;
  setpwent();
  while ( (pw = getpwent()) != NULL )
    {
      std::string userName = pw->pw_name;
      if (haveUserTable)
        {
          Table::const_iterator got = users.find(userName);
          if (got == users.end())
            continue;
          xaltDir.assign((*got).second);
        }
      else
        xaltDir.assign(pw->pw_dir);
      pbar.update(icnt++);

      // form directory xaltDir = "$HOME/.xalt.d"
      xaltDir.append("/.xalt.d");
      
      if (isDirectory(xaltDir.c_str()))
        {
          Vstring linkFnA;
          int nlink  = findFilesInDir(xaltDir, "link.*.json", linkFnA);
          lnkCnt    += link_json_fileA_to_db(options, rmapT, linkFnA);
          if (options.deleteFn())
            removeFiles(linkFnA);

          Vstring runFnA;
          int nrun = findFilesInDir(xaltDir, "run.*.json", runFnA);
          runCnt += run_json_fileA_to_db(options, rmapT, runFnA);
          if (options.deleteFn())
            removeFiles(runFnA);
      
          if (nlink + nrun > 0)
            iuser++;
        }
    }
  endpwent();

  pbar.fini();
  time_t rt = (time_t) (epoch() - t1);
  if (options.timer())
    {
      const int dateSz = 100;
      char dateStr[dateSz];
      strftime(dateStr, dateSz, "%T", gmtime(&rt));
      printf("Time: %s\n", dateStr);
    }

  printf("num users: %d, num links: %d, num runs: %d\n", iuser, lnkCnt, runCnt);
  return 0;
}
