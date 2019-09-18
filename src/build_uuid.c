#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xalt_obfuscate.h"
#include "build_uuid.h"
#include "xalt_fgets_alloc.h"
#include "xalt_config.h"
#include "uuid/uuid.h"
#ifdef HAVE_WORKING_LIBUUID
#  include <uuid/uuid.h>
   void build_uuid(char * my_uuid_str)
   {
     uuid_t uuid;
     uuid_generate_random(uuid);
     uuid_unparse_lower(uuid, my_uuid_str);
   }
#else
   void build_uuid(char * my_uuid_str)
   {
     const char* uuid_proc_fn = "/proc/sys/kernel/random/uuid";
     char*       buf          = NULL;
     size_t      sz           = 0;
     FILE*       fp           = fopen(uuid_proc_fn,"r");

     if (!fp)
       {
         fprintf(stderr,"Unable to open: %s\n",uuid_proc_fn);
         exit(1);
       }

     xalt_fgets_alloc(fp, &buf, &sz);
     memcpy(my_uuid_str,buf, 36);
     my_uuid_str[36] = '\0';
     fclose(fp);
     free(buf)
   }
#endif
