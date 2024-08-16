#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include "xalt_obfuscate.h"
#include "build_uuid.h"
#include "xalt_config.h"
#include "xalt_header.h"
#include "xalt_dir.h"
#include "xalt_c_utils.h"
#include <uuid/uuid.h>
#define BAD_UUID "deadbeaf-dead-beef-1111-deadbeef1111"

static void* handle = NULL;

       
#ifdef HAVE_FUNC_GETENTROPY
   int uuidv7(uint8_t *value)
   {
     // random bytes
     int err = getentropy(value, 16);
     if (err != EXIT_SUCCESS) 
       return EXIT_FAILURE;

     // current timestamp in seconds and nano-seconds
     struct timespec ts;
     int ok = timespec_get(&ts, TIME_UTC);
     if (ok == 0) 
       return EXIT_FAILURE;

     uint64_t timestamp = (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

     // timestamp
     value[0] = (timestamp >> 40) & 0xFF;
     value[1] = (timestamp >> 32) & 0xFF;
     value[2] = (timestamp >> 24) & 0xFF;
     value[3] = (timestamp >> 16) & 0xFF;
     value[4] = (timestamp >>  8) & 0xFF;
     value[5] = timestamp & 0xFF;

     // version and variant
     value[6] = (value[6] & 0x0F) | 0x70;
     value[8] = (value[8] & 0x3F) | 0x80;

     return EXIT_SUCCESS;
   }

   void uuidv7_unparse_lower(uint8_t* u, char* uuidStr)
   {
     //01234567-0123-0123-0123-012345678901
     //01915724-4c55-74cd-872a-bbb4d58bc892
     // 0 1 2 3  4 5  6 7  8 9  0 1 2 3 4 5
     //01234567-0123-0123-0123-012345678901
     //          1         2         3
     //        8901234567890123456789012345
     int tbl[] = {0, 4, 6, 8, 10, 16};
     int j = 0; 
     for (int k = 0; k < 5; ++k)
       {
         int istart = tbl[k];
         int iend   = tbl[k+1];
         for (int i = istart; i < iend; ++i)
           {
             sprintf(&uuidStr[j], "%02x", u[i]);
             j += 2;
           }
         uuidStr[j++] = '-';
       }
     uuidStr[36] = '\0';
   }

   void build_uuid(char * my_uuid_str)
   {
     uint8_t my_uuid[16];

     uuidv7(&my_uuid[0]);
     uuidv7_unparse_lower(&my_uuid[0], my_uuid_str);
   }   

#else
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
         if ( ! handle) 
           {
             handle = dlopen ("libuuid.so", RTLD_LAZY);
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
     /* Uncomment for testing duplicated uuid's
      * strcpy(my_uuid_str,"3ae43486-bfe2-11eb-8a3a-b376be588fbc");
      */
   }  
#endif

void build_uuid_cleanup()
{
  if (handle)
    dlclose(handle);
}
