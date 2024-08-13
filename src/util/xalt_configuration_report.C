#include "xalt_obfuscate.h"
#include "xalt_config.h"
#include "xalt_types.h"
#include "capture.h"
#include "xalt_syshost.h"
#include "xalt_config.h"
#include "xalt_dir.h"
#include "xalt_regex.h"
#include "xalt_version.h"
#include "xalt_interval.h"
#include "xalt_utils.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
#include "epoch.h"
#include "buildJson.h"
#include "utarray.h"


//the following are ASCII terminal color codes.
#define RESET       "\033[0m"
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */

static const char* blank0      = "";
static const char* comma       = ",";
const int dateSZ=100;

void displayArray(const char* config_py_fn, const char *name, int n, const char **A)
{
  std::cout << "*----------------------*\n";
  std::cout << " Array: " << name << "\n";
  std::cout << "*----------------------*\n";

  int j = 0;
  for (int i = 0; i < n; ++i)
    {
      if (strncmp(A[i],"====",4) == 0)
        std::cout << "================== src/tmpl/xalt_config.py ==================\n";
      else if (strncmp(A[i],"----",4) == 0)
        std::cout << "================== "<< config_py_fn << " ==================\n";
      else
        std::cout << std::setw(4) << j++ << ": " << A[i] << "\n";
    }
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


  UT_array*   resultA;
  std::string cmd      = "LD_PRELOAD= XALT_EXECUTABLE_TRACKING=no ";
  char* fn = xalt_dir("bin/xalt_print_os");
  cmd.append(fn);
  free(fn);
  capture(cmd.c_str(), &resultA);
  char **pp = NULL;
  pp = (char**) utarray_next(resultA, pp);
  std::string current_os_descript = *pp;

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
      (strcasecmp(transmission,"curl")      != 0 ))
    transmission = "file";

  const char *log_url = getenv("XALT_LOGGING_URL");
  if (log_url == NULL)
    log_url = XALT_LOGGING_URL;

  long        always_record     = xalt_mpi_always_record;
  const char *always_record_str = getenv("XALT_MPI_ALWAYS_RECORD");
  if (always_record_str)
    always_record = strtol(always_record_str, (char **) NULL, 10);

  const char* computeSHA1 = getenv("XALT_COMPUTE_SHA1");
  if (computeSHA1 == NULL)
    computeSHA1 = XALT_COMPUTE_SHA1;

  const char* cmdline_record = getenv("XALT_CMDLINE_RECORD");
  if (cmdline_record == NULL)
    cmdline_record = XALT_CMDLINE_RECORD;

  const char* xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  const char* xalt_uuid_v7 = XALT_UUID_V7;

  const char * rmapT_str = "false";
  bool found_rmapT = false;
  std::string rmapD = ""; // If this is an empty string then XALT_ETC_DIR is used to specify location of rmapD
  FILE *fp = xalt_json_file_open(rmapD, "reverseMapD/xalt_rmapT");
  if (fp)
    {
      fclose(fp);
      found_rmapT = true;
      rmapT_str   = "true";
    }

  const char* xalt_mpi_tracking = getenv("XALT_MPI_TRACKING");
  if (xalt_mpi_tracking == NULL)
    xalt_mpi_tracking = XALT_MPI_TRACKING;

  std::string gpu_style_str;
  const char* xalt_gpu_tracking = getenv("XALT_GPU_TRACKING");
  if (xalt_gpu_tracking == NULL)
    xalt_gpu_tracking = XALT_GPU_TRACKING;
  if ( strcasecmp(xalt_gpu_tracking,"no") == 0)
    gpu_style_str.assign("no");
  else
    gpu_style_str.assign("yes");
  bool have_gpu_monitoring = false;
  if (strcasecmp(HAVE_DCGM,"yes") == 0)
    {
      gpu_style_str.append("(dcgm)");
      have_gpu_monitoring = true;
    }
  if (strcasecmp(HAVE_NVML,"yes") == 0)
    {
      gpu_style_str.append("(nvml)");
      have_gpu_monitoring = true;
    }
  if ( ! have_gpu_monitoring && strcasecmp(xalt_gpu_tracking,"yes") == 0)
    gpu_style_str.append("(none)");

  const char* xalt_func_tracking = getenv("XALT_FUNCTION_TRACKING");
  if (xalt_func_tracking == NULL)
    xalt_func_tracking = XALT_FUNCTION_TRACKING;
  else if (strcmp(xalt_func_tracking,"no") != 0)
    xalt_func_tracking = "yes";
    
  const char* xalt_signal_handler = getenv("XALT_SIGNAL_HANDLER");
  if (xalt_signal_handler == NULL)
    xalt_signal_handler = XALT_SIGNAL_HANDLER;
  else if (strcasecmp(xalt_signal_handler,"yes") != 0)
    xalt_signal_handler = "no";
    
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
      char*       jsonStr;
      Json_t      json;
      const char* my_sep = blank0;
      json_init(Json_TABLE, &json);
      
      json_add_char_str(&json, my_sep,   "DATE",                     dateStr);              my_sep = comma;
      json_add_char_str(&json, my_sep,   "XALT_EXECUTABLE_TRACKING", executable_tracking);
      json_add_char_str(&json, my_sep,   "XALT_PRELOAD_ONLY",        xalt_preload_only);
      json_add_char_str(&json, my_sep,   "XALT_SYSHOST",             syshost.c_str());
      json_add_char_str(&json, my_sep,   "XALT_VERSION",             XALT_VERSION);
      json_add_char_str(&json, my_sep,   "XALT_INTERFACE_VERSION",   XALT_INTERFACE_VERSION);
      json_add_char_str(&json, my_sep,   "XALT_GIT_VERSION",         XALT_GIT_VERSION);
      json_add_char_str(&json, my_sep,   "XALT_VERSION_STR",         XALT_VERSION_STR);
      json_add_char_str(&json, my_sep,   "XALT_FILE_PREFIX",         XALT_FILE_PREFIX);
      json_add_char_str(&json, my_sep,   "XALT_TRANSMISSION_STYLE",  transmission);
      json_add_char_str(&json, my_sep,   "XALT_FUNCTION_TRACKING",   xalt_func_tracking);
      if (strcmp(transmission,"syslog") == 0)
        json_add_char_str(&json, my_sep, "XALT_LOGGING_TAG",         syslog_tag.c_str());
      if (strcmp(transmission,"curl") == 0)
        json_add_char_str(&json, my_sep, "XALT_LOGGING_URL",         log_url);
      json_add_int(     &json, my_sep,   "XALT_PRIME_NUMBER",        XALT_PRIME_NUMBER);
      json_add_char_str(&json, my_sep,   "XALT_COMPUTE_SHA1",        computeSHA1);
      json_add_char_str(&json, my_sep,   "XALT_ETC_DIR",             xalt_etc_dir);
      json_add_char_str(&json, my_sep,   "XALT_DIR",                 xalt_dir(NULL));
      json_add_char_str(&json, my_sep,   "SITE_CONTROLLED_PREFIX",   SITE_CONTROLLED_PREFIX);
      json_add_char_str(&json, my_sep,   "XALT_CONFIG_PY",           XALT_CONFIG_PY);
      json_add_char_str(&json, my_sep,   "XALT_CMDLINE_RECORD",      cmdline_record);
      json_add_char_str(&json, my_sep,   "XALT_SIGNAL_HANDLER",      xalt_signal_handler);
      json_add_char_str(&json, my_sep,   "XALT_UUID_V7",             xalt_uuid_v7);
      json_add_char_str(&json, my_sep,   "XALT_SYSTEM_PATH",         XALT_SYSTEM_PATH);
      json_add_char_str(&json, my_sep,   "XALT_SYSHOST_CONFIG",      SYSHOST_CONFIG);
      json_add_char_str(&json, my_sep,   "XALT_MPI_TRACKING",        xalt_mpi_tracking);
      json_add_char_str(&json, my_sep,   "XALT_GPU_TRACKING",        gpu_style_str.c_str());
      json_add_char_str(&json, my_sep,   "XALT_SCALAR_TRACKING",     xalt_scalar_tracking);
      json_add_char_str(&json, my_sep,   "XALT_SAMPLING",            xalt_sampling);
      json_add_int(     &json, my_sep,   "MPI_ALWAYS_RECORD",        (int) always_record);
      json_add_int(     &json, my_sep,   "XALT_SYSLOG_MSG_SZ",       SYSLOG_MSG_SZ);
      json_add_char_str(&json, my_sep,   "XALT_INSTALL_OS",          XALT_INSTALL_OS);
      json_add_char_str(&json, my_sep,   "XALT_CURRENT_OS",          current_os_descript.c_str());
      json_add_char_str(&json, my_sep,   "CXX_LD_LIBRARY_PATH",      cxx_ld_library_path.c_str());
      json_add_char_str(&json, my_sep,   "XALT_LD_LIBRARY_PATH",     XALT_LD_LIBRARY_PATH);
      json_add_char_str(&json, my_sep,   "HAVE_32BIT",               HAVE_32BIT);
      json_add_char_str(&json, my_sep,   "MY_HOSTNAME_PARSER",       MY_HOSTNAME_PARSER);
      json_add_char_str(&json, my_sep,   "HAVE_DCGM",                HAVE_DCGM);
      json_add_char_str(&json, my_sep,   "CRYPTO_STR",               CRYPTO_STR);
      json_add_char_str(&json, my_sep,   "UUID_STR",                 UUID_STR);
      json_add_char_str(&json, my_sep,   "GPU_STR",                  GPU_STR);
      json_add_char_str(&json, my_sep,   "CURL_STR",                 CURL_STR);
      json_add_char_str(&json, my_sep,   "FOUND_RmapT",              rmapT_str);

      json_add_array(&json, my_sep,   "hostnameA",       hostnameSz,       hostnameA);
      json_add_array(&json, my_sep,   "pathPatternA",    pathPatternSz,    pathPatternA);
      json_add_array(&json, my_sep,   "pathArgPatternA", pathArgPatternSz, pathArgPatternA);
      json_add_array(&json, my_sep,   "envPatternA",     envPatternSz,     envPatternA);
      json_add_array(&json, my_sep,   "pkgPatternA",     pkgPatternSz,     pkgPatternA);
      json_add_array(&json, my_sep,   "pyPkgPatternA",   pyPkgPatternSz,   pyPkgPatternA);
      json_add_array(&json, my_sep,   "ingestPatternA",  ingestPatternSz,  ingestPatternA);
      json_fini(&json, &jsonStr);

      std::cout << jsonStr << std::endl;
      free(jsonStr);
      return 0;
    }

  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "                      XALT Configuration Report\n";
  std::cout << "*------------------------------------------------------------------------------*\n\n";
  std::cout << "Today's DATE:                    " << dateStr                        << "\n";
  std::cout << "XALT_VERSION:                    " << XALT_VERSION                   << "\n";
  std::cout << "XALT_GIT_VERSION:                " << XALT_GIT_VERSION               << "\n";
  std::cout << "XALT_VERSION_STR:                " << XALT_VERSION_STR               << "\n";
  std::cout << "*------------------------------------------------------------------------------*\n";
  std::cout << "XALT_EXECUTABLE_TRACKING:        " << executable_tracking            << "\n";
  std::cout << "XALT_FUNCTION_TRACKING:          " << xalt_func_tracking             << "\n";
  std::cout << "XALT_SYSHOST:                    " << syshost                        << "\n";
  std::cout << "XALT_CMDLINE_RECORD:             " << cmdline_record                 << "\n";
  std::cout << "XALT_FILE_PREFIX:                " << XALT_FILE_PREFIX               << "\n";
  std::cout << "XALT_PRIME_NUMBER:               " << XALT_PRIME_NUMBER              << "\n";
  std::cout << "XALT_INTERFACE_VERSION:          " << XALT_INTERFACE_VERSION         << "\n";
  std::cout << "XALT_TRANSMISSION_STYLE:         " << transmission                   << "\n";
  if (strcmp(transmission,"syslog") == 0)
    std::cout << "XALT_LOGGING_TAG:                " << syslog_tag                   << "\n";
  if (strcmp(transmission,"curl") == 0)
    std::cout << "XALT_LOGGING_URL:                " << log_url                      << "\n";
  std::cout << "XALT_COMPUTE_SHA1 on libraries:  " << computeSHA1                    << "\n";
  std::cout << "XALT_ETC_DIR:                    " << xalt_etc_dir                   << "\n";
  std::cout << "XALT_DIR:                        " << xalt_dir(NULL)                 << "\n";
  std::cout << "SITE_CONTROLLED_PREFIX:          " << SITE_CONTROLLED_PREFIX         << "\n";
  std::cout << "XALT_CONFIG_PY:                  " << XALT_CONFIG_PY                 << "\n";
  std::cout << "XALT_MPI_TRACKING:               " << xalt_mpi_tracking              << "\n";
  std::cout << "XALT_GPU_TRACKING:               " << gpu_style_str                  << "\n";
  std::cout << "XALT_SCALAR_TRACKING:            " << xalt_scalar_tracking           << "\n";
  std::cout << "XALT_SAMPLING:                   " << xalt_sampling                  << "\n";
  std::cout << "MPI_ALWAYS_RECORD:               " << always_record                  << "\n";
  std::cout << "XALT_UUID_V7:                    " << xalt_uuid_v7                   << "\n";
  std::cout << "XALT_SIGNAL_HANDLER:             " << xalt_signal_handler            << "\n";
  std::cout << "XALT_SYSTEM_PATH:                " << XALT_SYSTEM_PATH               << "\n";
  std::cout << "XALT_SYSHOST_CONFIG:             " << SYSHOST_CONFIG                 << "\n";
  std::cout << "XALT_SYSLOG_MSG_SZ:              " << SYSLOG_MSG_SZ                  << "\n";
  std::cout << "CXX_LD_LIBRARY_PATH:             " << cxx_ld_library_path            << "\n";
  std::cout << "XALT_LD_LIBRARY_PATH:            " << XALT_LD_LIBRARY_PATH           << "\n";
  std::cout << "XALT_INSTALL_OS:                 " << XALT_INSTALL_OS                << "\n";
  std::cout << "CURRENT_OS:                      " << current_os_descript            << "\n";
  std::cout << "XALT_PRELOAD_ONLY:               " << XALT_PRELOAD_ONLY              << "\n";
  std::cout << "HAVE_32BIT:                      " << HAVE_32BIT                     << "\n";
  std::cout << "MY_HOSTNAME_PARSER:              " << MY_HOSTNAME_PARSER             << "\n";
  std::cout << "CRYPTO_STR:                      " << CRYPTO_STR                     << "\n";
  std::cout << "UUID_STR:                        " << UUID_STR                       << "\n";
  std::cout << "CURL_STR:                        " << CURL_STR                       << "\n";
  std::cout << "GPU_STR:                         " << GPU_STR                        << "\n";
  std::cout << "Built with DCGM:                 " << HAVE_DCGM                      << "\n";
  if (found_rmapT)
    std::cout << "Found xalt_rmapT.json:           " << rmapT_str                    << "\n";
  else
    std::cout << BOLDRED << "Found xalt_rmapT.json:           " << rmapT_str << RESET <<"\n";
    
  std::cout << "*------------------------------------------------------------------------------*\n\n";

  if (! found_rmapT)
    std::cout << "\n*------------------------------------------------------------------------------*\n"
              << BOLDRED << "Note: " << RESET
              << "Since XALT cannot find xalt_rmapT.json, function_tracking is turned off\n"
              << "*------------------------------------------------------------------------------*\n\n";
    



  std::cout << "*------------------------------------------------------------------------------*\n"
            << "Note: The matching patterns are from either your site's config.py file or\n"
            << "      from the src/tmpl/xalt_config.py file as marked below\n"
            << "*------------------------------------------------------------------------------*\n\n";

  displayArray(XALT_CONFIG_PY, "hostnameA",       hostnameSz,       hostnameA);
  std::cout << "\nRemember that \"PKGS\" means a program that can also track internal packages\n";
  displayArray(XALT_CONFIG_PY, "pathPatternA",    pathPatternSz,    pathPatternA);
  displayArray(XALT_CONFIG_PY, "pathArgPatternA", pathArgPatternSz, pathArgPatternA);
  displayArray(XALT_CONFIG_PY, "envPatternA",     envPatternSz,     envPatternA);
  displayArray(XALT_CONFIG_PY, "pkgPatternA",     pkgPatternSz,     pkgPatternA);
  displayArray(XALT_CONFIG_PY, "pyPkgPatternA",   pyPkgPatternSz,   pyPkgPatternA);
  displayArray(XALT_CONFIG_PY, "ingestPatternA",  ingestPatternSz,  ingestPatternA);

  std::cout << "*-------------------------------*\n";
  std::cout << " Array: Non-MPI (scalar) interval\n";
  std::cout << "*-------------------------------*\n";
  for (int i = 0; i < scalar_rangeSz-1; ++i)
    std::cout << "Time Range(seconds): [" << scalar_rangeA[i].left << ", " << scalar_rangeA[i+1].left
              << "]: probability: "<< scalar_rangeA[i].prob << "\n";
  std::cout << "\n";

  std::cout << "*----------------------*\n";
  std::cout << " Array: MPI interval\n";
  std::cout << "*----------------------*\n";
  for (int i = 0; i < mpi_rangeSz-1; ++i)
    std::cout << "Time Range(seconds): [" << mpi_rangeA[i].left << ", " << mpi_rangeA[i+1].left
              << "]: probability: "<< mpi_rangeA[i].prob << "\n";
  std::cout << "\n";
    
  return 0;
}
