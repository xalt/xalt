#define _GNU_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/syscall.h>
#include <linux/random.h>
#include "xalt_obfuscate.h"
#include "build_uuid.h"
#include "xalt_config.h"
#include "xalt_header.h"
#include "xalt_dir.h"
#include "xalt_c_utils.h"
#define BAD_UUID "deadbeaf-dead-beef-1111-deadbeef1111"

static void* handle = NULL;
static int (*getentropy_ptr)(     void *buffer, size_t length) = NULL;
static int (*have_getentropy_ptr)(void *buffer, size_t length) = NULL;

/* Implementation of getentropy based on the getrandom system call.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */


/* Write LENGTH bytes of randomness starting at BUFFER.  Return 0 on
   success and -1 on failure.  */
int simple_getentropy (void *buffer, size_t length)
{
  /* The interface is documented to return EIO for buffer lengths
     longer than 256 bytes.  */
  if (length > 256)
    {
      errno = EIO;
      return -1;
    }

  /* Try to fill the buffer completely.  Even with the 256 byte limit
     above, we might still receive an EINTR error (when blocking
     during boot).  */
  void *end = buffer + length;
  while (buffer < end)
    {
      /* NB: No cancellation point.  */
      ssize_t bytes = syscall(SYS_getrandom, buffer, end - buffer, 0);
      if (bytes < 0)
        {
          if (errno == EINTR)
            /* Try again if interrupted by a signal.  */
            continue;
          else
            return -1;
        }
      if (bytes == 0)
        {
          /* No more bytes available.  This should not happen under
             normal circumstances.  */
          errno = EIO;
          return -1;
        }
      /* Try again in case of a short read.  */
      buffer += bytes;
    }
  return 0;
}
       

int have_libc_getentropy_func()
{
  build_getentropy_ptr();
  return getentropy_ptr != simple_getentropy;
}


void build_getentropy_ptr()
{
  if (! getentropy_ptr) 
    {
      getentropy_ptr = dlsym(RTLD_NEXT, "getentropy");
      if (! getentropy_ptr)
        {
          getentropy_ptr = simple_getentropy;
        }
    }
}

int uuidv7(uint8_t *value)
{
  build_getentropy_ptr();

  // random bytes
  int err = getentropy_ptr(value, 16);
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
