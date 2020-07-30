#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "xalt_obfuscate.h"
#include "build_uuid.h"
#include "xalt_config.h"
#include "xalt_dir.h"
#include "xalt_c_utils.h"
#include <uuid/uuid.h>
#define BAD_UUID "deadbeaf-dead-beef-1111-deadbeef1111"

static void* handle                                  = NULL;
static void (*my_uuid_generate_random)(uuid_t)       = NULL;
static void (*my_uuid_unparse_lower)(uuid_t, char*)  = NULL;

void build_uuid(char * my_uuid_str)
{
  char *error;
  uuid_t uuid;

  if (! my_uuid_generate_random)
    {
      char* fn = xalt_dir("lib64/libuuid.so");
      handle = dlopen (fn, RTLD_LAZY);
      if (fn)
	my_free(fn, strlen(fn));
      if (!handle) 
        {
          char* fn = xalt_dir("lib64/libuuid.so");
          handle = dlopen (fn, RTLD_LAZY);
          if (fn)
            {
              memset(fn, '\0', strlen(fn));
              free(fn);
            }
          if (!handle) 
            {
              fputs (dlerror(), stderr);
              strcpy(my_uuid_str, BAD_UUID);
              return;
            }
        }
      my_uuid_generate_random = dlsym(handle,"uuid_generate_random");
      if ((error = dlerror()) != NULL) 
        {
          fputs(error, stderr);
          strcpy(my_uuid_str,BAD_UUID);
          return;
        }

      my_uuid_unparse_lower = dlsym(handle,"uuid_unparse_lower");
      if ((error = dlerror()) != NULL) 
        {
          fputs(error, stderr);
          strcpy(my_uuid_str,BAD_UUID);
          return;
        }
    }
    
  (*my_uuid_generate_random)(uuid);
  (*my_uuid_unparse_lower)(uuid, my_uuid_str);
}

void build_uuid_cleanup()
{
  if (handle)
    dlclose(handle);
}
