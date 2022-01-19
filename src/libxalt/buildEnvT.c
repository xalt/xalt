#include <stdlib.h>
#include <string.h>
#include "buildEnvT.h"
#include "xalt_env_parser.h"

void buildEnvT(char* env[], S2S_t ** envT)
{
  int i;
  for (i = 0; env[i] != NULL; ++i)
    {
      char* w = env[i];
      // Skip any exported bash functions
      if (strncmp(w,"BASH_FUNC_",10) == 0)
	continue; 

      char* p = strchr(w, '=');
      // Skip any exported bash functions
      if (strncmp(w,"() ",3) == 0)
	continue;
      
      if (p && keep_env_name(w))
	{
	  S2S_t* e = (S2S_t *) XMALLOC(sizeof(S2S_t));
	  utstring_new(e->key);
	  utstring_bincpy(e->key, w, p-w);

	  utstring_new(e->value);
	  utstring_bincpy(e->value, p+1, strlen(p+1));

	  HASH_ADD_KEYPTR(hh, *envT, utstring_body(e->key), utstring_len(e->key), e);
	}
    }
  env_parser_cleanup();
}
