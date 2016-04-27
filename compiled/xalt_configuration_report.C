#include "xalt_config.h"
#include "xalt_version.h"
#include <iostream>
#include <time.h>
#include "epoch.h"
#define DATESZ 100

int main(int argc, char* argv[])
{
  char    dateStr[DATESZ];
  time_t  now = (time_t) epoch();
  strftime(dateStr,DATESZ, "%c", localtime(&now));
  
  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "                      XALT Configuration Report\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";
  std::cout << "Today's DATE:              " << dateStr            << "\n";
  std::cout << "XALT_VERSION:              " << XALT_VERSION       << "\n";
  std::cout << "XALT_GIT_VERSION:          " << XALT_GIT_VERSION   << "\n";
  std::cout << "XALT_VERSION_STR:          " << XALT_VERSION_STR   << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";
  std::cout << "XALT_TRANSMISSION_STYLE:   " << TRANSMISSION       << "\n";
  std::cout << "XALT_ETC_DIR:              " << XALT_ETC_DIR       << "\n";
  std::cout << "XALT_CONFIG_PY:            " << XALT_CONFIG_PY     << "\n";
  std::cout << "XALT_SYSTEM_PATH:          " << XALT_SYSTEM_PATH   << "\n";
  std::cout << "XALT_SYSLOG_MSG_SZ:        " << SYSLOG_MSG_SZ      << "\n";
  std::cout << "HAVE_32BIT:                " << HAVE_32BIT         << "\n";

  return 0;
}
