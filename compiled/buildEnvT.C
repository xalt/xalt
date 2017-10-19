#include "run_submission.h"
#include <stdlib.h>
#include <string.h>
#include "xalt_utils.h"
#include "xalt_env_parser.h"

void buildEnvT(Options& options, char* env[], Table& envT)
{
  std::string n, v;

  for (int i = 0; env[i] != NULL; ++i)
    {
      char * w = env[i];
      char * p = strchr(w, '=');

      if (p)
        {
          n.assign(w, p - w);
          v.assign(p+1);
          envT[n] = v;
        }
    }


  std::string path = options.path();
  if (path.size() > 0)
    envT["PATH"] = path;

  std::string ldLibPath = options.ldLibPath();
  if (ldLibPath.size() > 0)
    envT["LD_LIBRARY_PATH"] = ldLibPath;
}

void filterEnvT(char* env[], Table& envT)
{
  std::string n;

  for (int i = 0; env[i] != NULL; ++i)
    {
      char * w = env[i];
      char * p = strchr(w, '=');

      if (p)
        {
          if (!keep_env_name(w))
            {
              n.assign(w, p - w);
              envT.erase(n);
            }
        }
    }
  env_parser_cleanup();
}


                  // free memory used by keep_env_name()
