#include "run_submission.h"
#include <string.h>

void buildEnvT(char* env[], Table& envT)
{
  std::string n, v;
  std::string::size_type s1, s2, s3, s4;

  for (int i = 0; env[i] != NULL; ++i)
    {
      char * w = env[i];
      char * p = strchr(w, '=');
      if (p) {
        n.assign(w, p - w);
        v.assign(p+1);
      }
      envT[n] = v;
    }
}
