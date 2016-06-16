#include "xalt_config.h"
#include "xalt_regex.h"
#include "xalt_version.h"
#include <iostream>
#include <iomanip>
#include <time.h>
#include "epoch.h"
#include "stdlib.h"

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

  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "                      XALT Configuration Report\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";
  std::cout << "Today's DATE:              " << dateStr              << "\n";
  std::cout << "XALT_VERSION:              " << XALT_VERSION         << "\n";
  std::cout << "XALT_GIT_VERSION:          " << XALT_GIT_VERSION     << "\n";
  std::cout << "XALT_VERSION_STR:          " << XALT_VERSION_STR     << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "XALT_TRANSMISSION_STYLE:   " << transmission         << "\n";
  std::cout << "XALT_ETC_DIR:              " << xalt_etc_dir         << "\n";
  std::cout << "XALT_CONFIG_PY:            " << XALT_CONFIG_PY       << "\n";
  std::cout << "XALT_SYSTEM_PATH:          " << XALT_SYSTEM_PATH     << "\n";
  std::cout << "XALT_SYSHOST_CONFIG:       " << SYSHOST_CONFIG       << "\n";
  std::cout << "XALT_SYSLOG_MSG_SZ:        " << SYSLOG_MSG_SZ        << "\n";
  std::cout << "HAVE_32BIT:                " << HAVE_32BIT           << "\n";
  std::cout << "HAVE_WORKING_LIBUUID:      " << HAVE_WORKING_LIBUUID << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";

  displayArray("acceptPathA", acceptPathSz, acceptPathA);
  displayArray("ignorePathA", ignorePathSz, ignorePathA);
  displayArray("hostnameA",   hostnameSz,   hostnameA);
  displayArray("acceptEnvA",  acceptEnvSz,  acceptEnvA);
  displayArray("ignoreEnvA",  ignoreEnvSz,  ignoreEnvA);

  return 0;
}
