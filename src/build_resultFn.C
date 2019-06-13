#define  _GNU_SOURCE
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "Options.h"
#include "xalt_config.h"

#define BITS_IN_int    (sizeof (int)*CHAR_BIT)
#define THREE_QUARTERS ((int) ((BITS_IN_int * 3) / 4))
#define ONE_EIGHTH     ((int) (BITS_IN_int / 8))
#define HIGH_BITS      ( ~ ((unsigned int) (~0) >> ONE_EIGHTH ))

unsigned int hashStr(const char * string)
{
  unsigned int hash_value, i;
  for ( hash_value = 0; *string; ++string)
    {
      hash_value = (hash_value << ONE_EIGHTH) + *string;
      if ( (i = hash_value & HIGH_BITS ) != 0)
        hash_value = (hash_value ^ (i >> THREE_QUARTERS )) & ~HIGH_BITS;
    }
  return hash_value;
}

void build_resultFn(std::string& resultDir, std::string& resultFn, Options& options, const char *kind, const char* transmission)
{
  char* c_home = getenv("HOME");
  char* c_user = getenv("USER");
  
  if (c_home != NULL && c_user != NULL )
    {
      #ifdef HAVE_FILE_PREFIX
        resultDir.assign(XALT_FILE_PREFIX);
        resultDir.append("/");
        if (strcasecmp(transmission,"file_separate_dirs") == 0)
          {
            resultDir.append(kind);
            resultDir.append("/");
          }
        char * hashDir = NULL;
        asprintf(&hashDir, XALT_PRIME_FMT,hashStr(c_user) % XALT_PRIME_NUMBER);
        resultDir.append(hashDir);
        free(hashDir);
      #else
        resultDir.assign(c_home);
        resultDir.append("/.xalt.d/");
        if (strcasecmp(transmission,"file_separate_dirs") == 0)
          {
            resultDir.append(kind);
            resultDir.append("/");
          }
      #endif
        
      double start = options.startTime();
      double frac  = start - floor(start);
      time = options.startTime();
      strftime(dateStr, DATESZ, "%Y_%m_%d_%H_%M_%S",localtime(&time));

      std::ostringstream sstream;
      sstream << kind << "." << options.syshost() << ".";
      sstream << dateStr << "_"    << std::setfill('0') << std::setw(4) << (int) (frac*10000.0) << "."
              << suffix  << "."    << options.uuid()    << ".json";

      resultFn = sstream.str();
    }
}
