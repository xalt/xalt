#include "xalt_obfuscate.h"
#include "xalt_config.h"
#include "xalt_types.h"
#include "capture.h"
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
      std::cout << "   Warning: XALT_EXECUTABLE_TRACKING is not set to \"yes\"!!                    \n";
      std::cout << "                                                                                \n";
      std::cout << "            XALT will not do anything without this variable                     \n";
      std::cout << "            set to \"yes\"                                                      \n";
      std::cout << "*------------------------------------------------------------------------------*\n";
      return 1;
    }
    
  const char* home = getenv("HOME");
  if (home == NULL)
    {
      std::cout << "*------------------------------------------------------------------------------*\n";
      std::cout << "   Warning: HOME is not set                                                     \n";
      std::cout << "                                                                                \n";
      std::cout << "            XALT will not do anything without this variable having a value      \n";
      std::cout << "*------------------------------------------------------------------------------*\n";
      return 1;
    }
    
  const char* user = getenv("USER");
  if (user == NULL)
    {
      std::cout << "*------------------------------------------------------------------------------*\n";
      std::cout << "   Warning: USER is not set                                                     \n";
      std::cout << "                                                                                \n";
      std::cout << "            XALT will not do anything without this variable having a value      \n";
      std::cout << "*------------------------------------------------------------------------------*\n";
      return 1;
    }
    

  if (argc > 1)
    {
      if (strcmp(argv[1],"--version") == 0)
        {
          std::cout << "XALT VERSION: " << XALT_VERSION << "\n";
          return 0;
        }

      if (strcmp(argv[1],"--git_version") == 0)
        {
          std::cout << "XALT GIT VERSION: " << XALT_GIT_VERSION << "\n";
          return 0;
        }
    }
        
  std::string syshost(xalt_syshost());
  std::string syslog_tag("XALT_LOGGING_");
  syslog_tag.append(syshost);


  Vstring     resultA;
  std::string cmd     = XALT_DIR "/bin/xalt_print_os";
  capture(cmd, resultA);
  std::string current_os_descript = resultA[0];

  char    dateStr[dateSZ];
  time_t  now = (time_t) epoch();
  strftime(dateStr,dateSZ, "%c", localtime(&now));
  
  const char* transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;
  if ((strcasecmp(transmission,"file")      != 0 ) &&
      (strcasecmp(transmission,"none")      != 0 ) && 
      (strcasecmp(transmission,"syslog")    != 0 ) && 
      (strcasecmp(transmission,"syslogv1")  != 0 ))
    transmission = "file";

  const char* computeSHA1 = getenv("XALT_COMPUTE_SHA1");
  if (computeSHA1 == NULL)
    computeSHA1 = XALT_COMPUTE_SHA1;

  const char* xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  const char* xalt_mpi_tracking = getenv("XALT_MPI_TRACKING");
  if (xalt_mpi_tracking == NULL)
    xalt_mpi_tracking = XALT_MPI_TRACKING;

  const char* xalt_gpu_tracking = getenv("XALT_GPU_TRACKING");
  if (xalt_gpu_tracking == NULL)
    xalt_gpu_tracking = XALT_GPU_TRACKING;
  if (strcmp(HAVE_DCGM,"no") == 0)
    xalt_gpu_tracking = HAVE_DCGM;
          
  const char* xalt_func_tracking = getenv("XALT_FUNCTION_TRACKING");
  if (xalt_func_tracking == NULL)
    xalt_func_tracking = XALT_FUNCTION_TRACKING;
  else if (strcmp(xalt_func_tracking,"no") != 0)
    xalt_func_tracking = "yes";
    
  const char* xalt_scalar_tracking = getenv("XALT_SCALAR_TRACKING");
  if (xalt_scalar_tracking == NULL)
    xalt_scalar_tracking = XALT_SCALAR_TRACKING;

  const char* xalt_sampling = getenv("XALT_SAMPLING");
  if (!xalt_sampling)
    {
      xalt_sampling = getenv("XALT_SCALAR_SAMPLING");
      if (!xalt_sampling)
        xalt_sampling = getenv("XALT_SCALAR_AND_SPSR_SAMPLING");
    }
  if (xalt_sampling == NULL || strcmp(xalt_sampling,"yes") != 0)
    xalt_sampling = "no";

  const char* xalt_preload_only   = XALT_PRELOAD_ONLY;

  std::string cxx_ld_library_path = CXX_LD_LIBRARY_PATH;
  if (cxx_ld_library_path == "")
    cxx_ld_library_path = "<empty>";

  if (argc == 2 && strcmp(argv[1],"--json") == 0) 
    {
      Json json;
      json.add("DATE",                          dateStr);
      json.add("XALT_EXECUTABLE_TRACKING",      executable_tracking);
      json.add("XALT_PRELOAD_ONLY",             xalt_preload_only);
      json.add("XALT_SYSHOST",                  syshost);
      json.add("XALT_VERSION",                  XALT_VERSION);
      json.add("XALT_INTERFACE_VERSION",        XALT_INTERFACE_VERSION);
      json.add("XALT_GIT_VERSION",              XALT_GIT_VERSION);
      json.add("XALT_VERSION_STR",              XALT_VERSION_STR);
      json.add("XALT_FILE_PREFIX",              XALT_FILE_PREFIX);
      json.add("XALT_TRANSMISSION_STYLE",       transmission);
      json.add("XALT_FUNCTION_TRACKING",        xalt_func_tracking);
      if (strcmp(transmission,"syslog") == 0)
        json.add("XALT_LOGGING_TAG",            syslog_tag);
      json.add("XALT_PRIME_NUMBER",             XALT_PRIME_NUMBER);
      json.add("XALT_COMPUTE_SHA1",             computeSHA1);
      json.add("XALT_ETC_DIR",                  xalt_etc_dir);
      json.add("XALT_DIR",                      XALT_DIR);
      json.add("BAD_INSTALL",                   BAD_INSTALL);
      json.add("XALT_CONFIG_PY",                XALT_CONFIG_PY);
      json.add("XALT_SYSTEM_PATH",              XALT_SYSTEM_PATH);
      json.add("XALT_SYSHOST_CONFIG",           SYSHOST_CONFIG);
      json.add("XALT_MPI_TRACKING",             xalt_mpi_tracking);
      json.add("XALT_GPU_TRACKING",             xalt_gpu_tracking);
      json.add("XALT_SCALAR_TRACKING",          xalt_scalar_tracking);
      json.add("XALT_SAMPLING",                 xalt_sampling);
      json.add("MPI_ALWAYS_RECORD",             mpi_always_record);
      json.add("XALT_SYSLOG_MSG_SZ",            SYSLOG_MSG_SZ);
      json.add("XALT_INSTALL_OS",               XALT_INSTALL_OS);
      json.add("XALT_CURRENT_OS",               current_os_descript);
      json.add("CXX_LD_LIBRARY_PATH",           cxx_ld_library_path);
      json.add("HAVE_32BIT",                    HAVE_32BIT);
      json.add("MY_HOSTNAME_PARSER",            MY_HOSTNAME_PARSER);
      json.add("HAVE_DCGM",                     HAVE_DCGM);
      json.add("CRYPTO_STR",                    CRYPTO_STR);
      json.add("UUID_STR",                      UUID_STR);

      json.add("hostnameA",    hostnameSz,      hostnameA);
      json.add("pathPatternA", pathPatternSz,   pathPatternA);
      json.add("envPatternA",  envPatternSz,    envPatternA);
      json.fini();

      std::string jsonStr = json.result();
      std::cout << jsonStr << std::endl;
      return 0;
    }

  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "                      XALT Configuration Report\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";
  std::cout << "Today's DATE:                  " << dateStr                        << "\n";
  std::cout << "XALT_VERSION:                  " << XALT_VERSION                   << "\n";
  std::cout << "XALT_GIT_VERSION:              " << XALT_GIT_VERSION               << "\n";
  std::cout << "XALT_VERSION_STR:              " << XALT_VERSION_STR               << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "XALT_EXECUTABLE_TRACKING:      " << executable_tracking            << "\n";
  std::cout << "XALT_FUNCTION_TRACKING:        " << xalt_func_tracking             << "\n";
  std::cout << "XALT_SYSHOST:                  " << syshost                        << "\n";
  std::cout << "XALT_FILE_PREFIX:              " << XALT_FILE_PREFIX               << "\n";
  std::cout << "XALT_PRIME_NUMBER:             " << XALT_PRIME_NUMBER              << "\n";
  std::cout << "XALT_INTERFACE_VERSION:        " << XALT_INTERFACE_VERSION         << "\n";
  std::cout << "XALT_TRANSMISSION_STYLE:       " << transmission                   << "\n";
  if (strcmp(transmission,"syslog") == 0)
    std::cout << "XALT_LOGGING_TAG:              " << syslog_tag                   << "\n";
  std::cout << "XALT_COMPUTE_SHA1:             " << computeSHA1                    << "\n";
  std::cout << "XALT_ETC_DIR:                  " << xalt_etc_dir                   << "\n";
  std::cout << "XALT_DIR:                      " << XALT_DIR                       << "\n";
  std::cout << "BAD_INSTALL:                   " << BAD_INSTALL                    << "\n";
  std::cout << "XALT_CONFIG_PY:                " << XALT_CONFIG_PY                 << "\n";
  std::cout << "XALT_MPI_TRACKING:             " << xalt_mpi_tracking              << "\n";
  std::cout << "XALT_GPU_TRACKING:             " << xalt_gpu_tracking              << "\n";
  std::cout << "XALT_SCALAR_TRACKING:          " << xalt_scalar_tracking           << "\n";
  std::cout << "XALT_SAMPLING:                 " << xalt_sampling                  << "\n";
  std::cout << "MPI_ALWAYS_RECORD:             " << mpi_always_record              << "\n";
  std::cout << "XALT_SYSTEM_PATH:              " << XALT_SYSTEM_PATH               << "\n";
  std::cout << "XALT_SYSHOST_CONFIG:           " << SYSHOST_CONFIG                 << "\n";
  std::cout << "XALT_SYSLOG_MSG_SZ:            " << SYSLOG_MSG_SZ                  << "\n";
  std::cout << "CXX_LD_LIBRARY_PATH:           " << cxx_ld_library_path            << "\n";
  std::cout << "XALT_INSTALL_OS:               " << XALT_INSTALL_OS                << "\n";
  std::cout << "CURRENT_OS:                    " << current_os_descript            << "\n";
  std::cout << "XALT_PRELOAD_ONLY:             " << XALT_PRELOAD_ONLY              << "\n";
  std::cout << "HAVE_32BIT:                    " << HAVE_32BIT                     << "\n";
  std::cout << "MY_HOSTNAME_PARSER:            " << MY_HOSTNAME_PARSER             << "\n";
  std::cout << "CRYPTO_STR:                    " << CRYPTO_STR                     << "\n";
  std::cout << "UUID_STR:                      " << UUID_STR                       << "\n";
  std::cout << "Built with DCGM:               " << HAVE_DCGM                      << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";

  displayArray("hostnameA",    hostnameSz,    hostnameA);
  std::cout << "\nRemember that \"PKGS\" means a program that can also track internal packages\n";
  displayArray("pathPatternA", pathPatternSz, pathPatternA);
  displayArray("envPatternA",  envPatternSz,  envPatternA);

  std::cout << "*----------------------*\n";
  std::cout << " Array: interval\n";
  std::cout << "*----------------------*\n";
  for (int i = 0; i < rangeSz-1; ++i)
    std::cout << "Time Range(seconds): [" << rangeA[i].left << ", " << rangeA[i+1].left
              << "]: probability: "<< rangeA[i].prob << "\n";
  std::cout << "\n";
    
  return 0;
}
