#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/syscall.h>
#include <linux/random.h>

#include "mpi.h"

static void* handle = NULL;
static int (*getentropy_ptr)(void *buffer, size_t length) = NULL;
int ierr, myProc, nProc;



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

static char *s1 = "Using internal getentropy";
static char *s2 = "Using libc getentropy";

void build_getentropy_ptr()
{
  char *s = s2;
  getentropy_ptr = dlsym(RTLD_NEXT, "getentropy");
  if (! getentropy_ptr)
    {
      getentropy_ptr = simple_getentropy;
      s = s1;
    }
  if (myProc == 0)
    printf("%s\n",s);
}

int uuidv7(uint8_t *value)
{
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


void uuid7_unparse_lower(uint8_t* u, char* uuidStr)
{
  //01234567-0123-0123-0123-012345678901
  //f9b8dd57-628d-41de-9e7b-2f1f893be268
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
int charPAcmp(const void* x, const void* y)
{
  char** a = (char **) x;
  char** b = (char **) y;
  int    v = strcmp(*a,*b);
  return v;
}


int main(int argc, char* argv[])
{
  const int sz = 10;
  uint8_t uuidA[sz][16];
  unsigned char uuidStr[37];
  
  ierr = MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myProc);
  MPI_Comm_size(MPI_COMM_WORLD, &nProc);

  build_getentropy_ptr();


  for (int j = 0; j < sz; ++j)
    uuidv7(&uuidA[j][0]);
      
  int strSz = 37*sz*nProc;

  unsigned char*  snd_block    = (unsigned char*)  malloc(strSz*sizeof(unsigned char));
  unsigned char** snd_uuidStrA = (unsigned char**) malloc(sz*nProc*sizeof(unsigned char*));
  memset(snd_block, 0, strSz);

  unsigned char*  rcv_block    = (unsigned char*)  malloc(strSz*sizeof(unsigned char));
  unsigned char** rcv_uuidStrA = (unsigned char**) malloc(sz*nProc*sizeof(unsigned char*));
  memset(rcv_block, 0, strSz);

  int j = 0;
  for (int i = 0; i < sz*nProc; ++i)
    {
      snd_uuidStrA[i] = &snd_block[j];
      rcv_uuidStrA[i] = &rcv_block[j];
      j += 37;
    }
  

  int idx = sz*myProc;
  for (int j = 0; j < sz; ++j)
    uuid7_unparse_lower(&uuidA[j][0], &snd_uuidStrA[idx+j][0]);

  MPI_Allreduce(&snd_uuidStrA[0][0], &rcv_uuidStrA[0][0], strSz, MPI_UNSIGNED_CHAR,
                MPI_SUM, MPI_COMM_WORLD);
  
  qsort((void *) rcv_uuidStrA, (size_t) sz*nProc, sizeof(unsigned char *), charPAcmp);


  if (myProc == 0)
    {
      for (int j = 0; j < sz*nProc; ++j)
        printf("%s\n", rcv_uuidStrA[j]);
      
      for (int j = 1; j < sz*nProc; ++j)
        {
          if (strcmp(rcv_uuidStrA[j-1],rcv_uuidStrA[j]) == 0)
            printf("found dup at %d\n", j-1);
        }

      printf("all done on %d procs\n",nProc);
    }

  MPI_Finalize();

  return 0;
}
