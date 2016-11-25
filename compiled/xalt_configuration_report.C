#include "xalt_config.h"
#include "xalt_regex.h"
#include "xalt_version.h"
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "epoch.h"
#include "Json.h"

const int dateSZ=100;

void displayArray(const char *name, int n, const char **A)
{
  std::cout << "*----------------------*\n";
  std::cout << " Array: " << name << "\n";
  std::cout << "*----------------------*\n";

  for (int i = 0; i < n; ++i)
    std::cout << std::setw(4) << i << ": " << A[i] << "\n";
  std::cout << "\n";
}


int main(int argc, char* argv[])
{
  char    dateStr[dateSZ];
  time_t  now = (time_t) epoch();
  strftime(dateStr,dateSZ, "%c", localtime(&now));
  
  const char* transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

  const char* xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  if (argc == 2 && strcmp(argv[1],"--json") == 0) 
    {

      Json json;
      json.add("date",                    dateStr);
      json.add("XALT_VERSION",            XALT_VERSION);
      json.add("XALT_GIT_VERSION",        XALT_GIT_VERSION);
      json.add("XALT_VERSION_STR",        XALT_VERSION_STR);
      json.add("XALT_FILE_PREFIX",        XALT_FILE_PREFIX);
      json.add("XALT_TRANSMISSION_STYLE", transmission);
      json.add("XALT_ETC_DIR",            xalt_etc_dir);
      json.add("XALT_CONFIG_PY",          XALT_CONFIG_PY);
      json.add("XALT_SYSTEM_PATH",        XALT_SYSTEM_PATH);
      json.add("XALT_SYSHOST_CONFIG",     XALT_SYSHOST_CONFIG);
      json.add("XALT_SYSLOG_MSG_SZ",      XALT_SYSLOG_MSG_SZ);
      json.add("HAVE_32BIT",              HAVE_32BIT);
      json.add("USING_LIBUUID",           HAVE_WORKING_LIBUUID);
      json.add("BUILT_W_MySQL",           BUILT_W_MySQL);

      json.add("acceptPathA", acceptPathSz, acceptPathA);
      json.add("ignorePathA", ignorePathSz, ignorePathA);
      json.add("hostnameA",   hostnameSz,   hostnameA);
      json.add("acceptEnvA",  acceptEnvSz,  acceptEnvA);
      json.add("ignoreEnvA",  ignoreEnvSz,  ignoreEnvA);
      json.fini();

      std::string jsonStr = json.result();
      std::cout << jsonStr << std::endl;
      return 0;
    }

  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "                      XALT Configuration Report\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";
  std::cout << "Today's DATE:              " << dateStr              << "\n";
  std::cout << "XALT_VERSION:              " << XALT_VERSION         << "\n";
  std::cout << "XALT_GIT_VERSION:          " << XALT_GIT_VERSION     << "\n";
  std::cout << "XALT_VERSION_STR:          " << XALT_VERSION_STR     << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "XALT_FILE_PREFIX:          " << XALT_FILE_PREFIX     << "\n";
  std::cout << "XALT_TRANSMISSION_STYLE:   " << transmission         << "\n";
  std::cout << "XALT_ETC_DIR:              " << xalt_etc_dir         << "\n";
  std::cout << "XALT_CONFIG_PY:            " << XALT_CONFIG_PY       << "\n";
  std::cout << "XALT_SYSTEM_PATH:          " << XALT_SYSTEM_PATH     << "\n";
  std::cout << "XALT_SYSHOST_CONFIG:       " << SYSHOST_CONFIG       << "\n";
  std::cout << "XALT_SYSLOG_MSG_SZ:        " << SYSLOG_MSG_SZ        << "\n";
  std::cout << "HAVE_32BIT:                " << HAVE_32BIT           << "\n";
  std::cout << "Using libuuid:             " << HAVE_WORKING_LIBUUID << "\n";
  std::cout << "Built with MySQL:          " << BUILT_W_MySQL        << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";

  displayArray("acceptPathA", acceptPathSz, acceptPathA);
  displayArray("ignorePathA", ignorePathSz, ignorePathA);
  displayArray("hostnameA",   hostnameSz,   hostnameA);
  displayArray("acceptEnvA",  acceptEnvSz,  acceptEnvA);
  displayArray("ignoreEnvA",  ignoreEnvSz,  ignoreEnvA);

  return 0;
}
