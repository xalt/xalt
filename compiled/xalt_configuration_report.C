#include "xalt_obfuscate.h"
#include "xalt_syshost.h"
#include "xalt_config.h"
#include "xalt_regex.h"
#include "xalt_version.h"
#include "xalt_interval.h"
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
  const char* executable_tracking = getenv("XALT_EXECUTABLE_TRACKING");
  if (executable_tracking == NULL || (strcmp(executable_tracking,"yes") != 0))
    {
      std::cout << "*------------------------------------------------------------------------------*\n";
      std::cout << "   Warning: XALT_EXECUTABLE_TRACKING is not set to \"yes\"!!\n";
      std::cout << "\n";
      std::cout << "            XALT will not do anything without this variable\n";
      std::cout << "            set to \"yes\"\n";
      std::cout << "*------------------------------------------------------------------------------*\n";
      return 1;
    }
    
  std::string syshost(xalt_syshost());
  std::string syslog_tag("XALT_LOGGING_");
  syslog_tag.append(syshost);


  char    dateStr[dateSZ];
  time_t  now = (time_t) epoch();
  strftime(dateStr,dateSZ, "%c", localtime(&now));
  
  const char* transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;
  if ((strcasecmp(transmission,"file")      != 0 ) &&
      (strcasecmp(transmission,"none")      != 0 ) && 
      (strcasecmp(transmission,"syslog")    != 0 ) && 
      (strcasecmp(transmission,"syslogv1")  != 0 ) && 
      (strcasecmp(transmission,"direct2db") != 0 ))
    transmission = "file";

  const char* computeSHA1 = getenv("XALT_COMPUTE_SHA1");
  if (computeSHA1 == NULL)
    computeSHA1 = XALT_COMPUTE_SHA1;

  const char* enable_backgrounding = getenv("XALT_ENABLE_BACKGROUNDING");
  if (enable_backgrounding == NULL)
    enable_backgrounding = XALT_ENABLE_BACKGROUNDING;

  const char* xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  const char* xalt_mpi_tracking = getenv("XALT_MPI_TRACKING");
  if (xalt_mpi_tracking == NULL)
    xalt_mpi_tracking = XALT_MPI_TRACKING;

  const char* xalt_func_tracking = getenv("XALT_FUNCTION_TRACKING");
  if (xalt_func_tracking == NULL)
    xalt_func_tracking = XALT_FUNCTION_TRACKING;
  else if (strcmp(xalt_func_tracking,"no") != 0)
    xalt_func_tracking = "yes";
    
  const char* xalt_spsr_tracking = getenv("XALT_SPSR_TRACKING");
  if (xalt_spsr_tracking == NULL)
    xalt_spsr_tracking = XALT_SPSR_TRACKING;

  const char* xalt_scalar_tracking = getenv("XALT_SCALAR_TRACKING");
  if (xalt_scalar_tracking == NULL)
    xalt_scalar_tracking = XALT_SCALAR_TRACKING;

  const char* xalt_scalar_sampling = getenv("XALT_SCALAR_SAMPLING");
  if (xalt_scalar_tracking == NULL || strcmp(xalt_scalar_sampling,"yes") != 0)
    xalt_scalar_sampling = "no";
  


  if (argc == 2 && strcmp(argv[1],"--json") == 0) 
    {
      Json json;
      json.add("DATE",                        dateStr);
      json.add("XALT_EXECUTABLE_TRACKING",    executable_tracking);
      json.add("XALT_SYSHOST",                syshost);
      json.add("XALT_VERSION",                XALT_VERSION);
      json.add("XALT_INTERFACE_VERSION",      XALT_INTERFACE_VERSION);
      json.add("XALT_GIT_VERSION",            XALT_GIT_VERSION);
      json.add("XALT_VERSION_STR",            XALT_VERSION_STR);
      json.add("XALT_FILE_PREFIX",            XALT_FILE_PREFIX);
      json.add("XALT_ENABLE_BACKGROUNDING",   enable_backgrounding);
      json.add("XALT_TRANSMISSION_STYLE",     transmission);
      json.add("XALT_FUNCTION_TRACKING",      xalt_func_tracking);
      if (strcmp(transmission,"syslog") == 0)
        json.add("XALT_LOGGING_TAG",          syslog_tag);
      json.add("XALT_COMPUTE_SHA1",           computeSHA1);
      json.add("XALT_ETC_DIR",                xalt_etc_dir);
      json.add("XALT_CONFIG_PY",              XALT_CONFIG_PY);
      json.add("XALT_SYSTEM_PATH",            XALT_SYSTEM_PATH);
      json.add("XALT_SYSHOST_CONFIG",         SYSHOST_CONFIG);
      json.add("XALT_MPI_TRACKING",           xalt_mpi_tracking);
      json.add("XALT_SPSR_TRACKING",          xalt_spsr_tracking);
      json.add("XALT_SCALAR_TRACKING",        xalt_scalar_tracking);
      json.add("XALT_SCALAR_SAMPLING",        xalt_scalar_sampling);
      json.add("XALT_SYSLOG_MSG_SZ",          SYSLOG_MSG_SZ);
      json.add("CXX_LD_LIBRARY_PATH",         CXX_LD_LIBRARY_PATH);
      json.add("HAVE_32BIT",                  HAVE_32BIT);
      json.add("USING_LIBUUID",               HAVE_WORKING_LIBUUID);
      json.add("MY_HOSTNAME_PARSER",          MY_HOSTNAME_PARSER);
      json.add("BUILT_W_MySQL",               BUILT_W_MySQL);

      json.add("hostnameA",    hostnameSz,    hostnameA);
      json.add("pathPatternA", pathPatternSz, pathPatternA);
      json.add("envPatternA",  envPatternSz,  envPatternA);
      json.fini();

      std::string jsonStr = json.result();
      std::cout << jsonStr << std::endl;
      return 0;
    }

  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "                      XALT Configuration Report\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";
  std::cout << "Today's DATE:              " << dateStr                << "\n";
  std::cout << "XALT_VERSION:              " << XALT_VERSION           << "\n";
  std::cout << "XALT_GIT_VERSION:          " << XALT_GIT_VERSION       << "\n";
  std::cout << "XALT_VERSION_STR:          " << XALT_VERSION_STR       << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "XALT_EXECUTABLE_TRACKING:  " << executable_tracking    << "\n";
  std::cout << "XALT_FUNCTION_TRACKING:    " << xalt_func_tracking     << "\n";
  std::cout << "XALT_SYSHOST:              " << syshost                << "\n";
  std::cout << "XALT_FILE_PREFIX:          " << XALT_FILE_PREFIX       << "\n";
  std::cout << "XALT_INTERFACE_VERSION:    " << XALT_INTERFACE_VERSION << "\n";
  std::cout << "XALT_TRANSMISSION_STYLE:   " << transmission           << "\n";
  if (strcmp(transmission,"syslog") == 0)
    std::cout << "XALT_LOGGING_TAG:          " << syslog_tag           << "\n";
  std::cout << "XALT_COMPUTE_SHA1:         " << computeSHA1            << "\n";
  std::cout << "XALT_ENABLE_BACKGROUNDING: " << enable_backgrounding   << "\n";
  std::cout << "XALT_ETC_DIR:              " << xalt_etc_dir           << "\n";
  std::cout << "XALT_CONFIG_PY:            " << XALT_CONFIG_PY         << "\n";
  std::cout << "XALT_MPI_TRACKING:         " << xalt_mpi_tracking      << "\n";
  std::cout << "XALT_SPSR_TRACKING:        " << xalt_spsr_tracking     << "\n";
  std::cout << "XALT_SCALAR_TRACKING:      " << xalt_scalar_tracking   << "\n";
  std::cout << "XALT_SCALAR_SAMPLING:      " << xalt_scalar_sampling   << "\n";
  std::cout << "XALT_SYSTEM_PATH:          " << XALT_SYSTEM_PATH       << "\n";
  std::cout << "XALT_SYSHOST_CONFIG:       " << SYSHOST_CONFIG         << "\n";
  std::cout << "XALT_SYSLOG_MSG_SZ:        " << SYSLOG_MSG_SZ          << "\n";
  std::cout << "CXX_LD_LIBRARY_PATH:       " << CXX_LD_LIBRARY_PATH    << "\n";
  std::cout << "HAVE_32BIT:                " << HAVE_32BIT             << "\n";
  std::cout << "MY_HOSTNAME_PARSER:        " << MY_HOSTNAME_PARSER     << "\n";
  std::cout << "Using libuuid:             " << HAVE_WORKING_LIBUUID   << "\n";
  std::cout << "Built with MySQL:          " << BUILT_W_MySQL          << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";

  displayArray("hostnameA",    hostnameSz,    hostnameA);
  std::cout << "\nRemember that \"SPSR\" means a scalar program that creates a start record\n";
  displayArray("pathPatternA", pathPatternSz, pathPatternA);
  displayArray("envPatternA",  envPatternSz,  envPatternA);

  std::cout << "*----------------------*\n";
  std::cout << " Array: interval\n";
  std::cout << "*----------------------*\n";
  for (int i = 0; i < rangeSz-1; ++i)
    std::cout << "Range: [" << rangeA[i].left << ", " rangeA[i+1].left
              << "]: probability: "<< rangeA[i].prob << "\n";
  std::cout << "\n";
    
  return 0;
}
