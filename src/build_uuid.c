#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xalt_obfuscate.h"
#include "build_uuid.h"
#include "xalt_fgets_alloc.h"
#include "xalt_config.h"
#include "uuid.h"
void build_uuid(char * my_uuid_str)
{
  void *handle;
  char *error;
  uuid_t uuid;
  void (*uuid_generate_random)(uuid_t);
  void (*uuid_unparse_lower)(uuid_t, char*);

  handle = dlopen ("libuuid.so.1", RTLD_LAZY);
  if (!handle) 
    {
      handle = dlopen (XALT_DIR "lib64/libuuid.so", RTLD_LAZY);
      if (!handle) 
        {
          fputs (dlerror(), stderr);
          strcpy(my_uuid_str,"deadbeaf-dead-beef-1111-deadbeef1111");
          return;
        }
    }
  uuid_generate_random = dlsym(handle,"uuid_generate_random");
  if ((error = dlerror()) != NULL) 
    {
      fputs(error, stderr);
      strcpy(my_uuid_str,"deadbeaf-dead-beef-1111-deadbeef1111");
      return;
    }
  
  uuid_unparse_lower = dlsym(handle,"uuid_unparse_lower");
  if ((error = dlerror()) != NULL) 
    {
      fputs(error, stderr);
      strcpy(my_uuid_str,"deadbeaf-dead-beef-1111-deadbeef1111");
      return;
    }
    
  (*uuid_generate_random)(uuid);
  (*uuid_unparse_lower)(uuid, my_uuid_str);
}
