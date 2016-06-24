#include "run_submission.h"
#include <stdlib.h>
#include <string.h>

void buildEnvT(char* env[], Table& envT)
{
  std::string n, v;
  std::string::size_type s1, s2, s3, s4;

  for (int i = 0; env[i] != NULL; ++i)
    {
      char * w = env[i];
      char * p = strchr(w, '=');

      // If a variable starts with __XALT_ then ignore it
      if (strncmp(w, "__XALT_",7) == 0)
        continue;

      if (p) {
        n.assign(w, p - w);
        v.assign(p+1);
      }
      envT[n] = v;
    }
  const char * envA[] = {       "PATH",         "LD_LIBRARY_PATH"  };
  const char * envB[] = {"__XALT_PATH_", "__XALT_LD_LIBRARY_PATH_" };
  size_t       envSz  = sizeof(envA)/sizeof(envA[0]);

  for (int i = 0; i < envSz; ++i)
    {
      char *v = getenv(envB[i]);
      if (v)
        envT[envA[i]] = v;
    }
 }
