#include <stdlib.h>
#include <string.h>
#include "buildEnvT.h"
#include "xalt_env_parser.h"

void buildEnvT(char* env[], S2S_t ** envT)
{
  for (int i = 0; env[i] != NULL; ++i)
    {
      char* w = env[i];
      // Skip any exported bash functions
      if (strncmp(w,"BASH_FUNC_",10) == 0)
	continue; 

      char* p = strchr(w, '=');
      // Skip any exported bash functions
      if (strncmp(w,"() ",3) == 0)
	continue;
      
      if (p)
	{
	  S2S_t* e = (S2S_t *) malloc(sizeof(S2S_t));
	  utstring_new(e->key);
	  utstring_bincpy(e->key, w, p-w);

	  utstring_new(e->value);
	  utstring_bincpy(e->value, p+1, strlen(p+1));

	  HASH_ADD_KEYPTR(hh, *envT, utstring_body(e->key), utstring_len(e->key), e);
	}
    }
}

void filterEnvT(char* env[], S2S_t ** envT)
{
  S2S_t*     entry;
  UT_string* name;

  utstring_new(name);
  for (int i = 0; env[i] != NULL; ++i)
    {
      char* w = env[i];
      char* p = strchr(w, '=');
      
      if (p)
	{
	  if (!keep_env_name(w))
	    {
	      utstring_clear( name);
	      utstring_bincpy(name, w, p - w);
	      HASH_FIND_STR(*envT, utstring_body(name), entry);
	      if (entry)
		{
		  HASH_DELETE(hh, *envT, entry);
		  utstring_free(entry->key);
		  utstring_free(entry->value);
		  free(entry);
		}
	    }
	}
    }
  env_parser_cleanup();
  utstring_free(name);
}
