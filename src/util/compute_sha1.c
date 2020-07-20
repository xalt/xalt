#include "xalt_config.h"
#include "compute_sha1.h"
#include "xalt_dir.h"
#include "xalt_c_utils.h"
#include <fcntl.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>

static void (*sha1)(const unsigned char *d, size_t n, unsigned char *md) = NULL;

static void * handle = NULL;

void compute_sha1(const char* fn, char* sha1buf)
{
  struct stat    st;
  int            fd, i;
  char *         error;
  unsigned long  fileSz;
  unsigned char* buffer;
  unsigned char  hash[SHA_DIGEST_LENGTH];

  fd     = open(fn, O_RDONLY);
  fstat(fd, &st);
  fileSz = st.st_size;

  buffer = (unsigned char *) mmap(0, fileSz, PROT_READ, MAP_SHARED, fd, 0);
  if (buffer == MAP_FAILED)
    {
      close(fd);
      perror("Error mmapping the file");
      exit(EXIT_FAILURE);
    }

  if (sha1 == NULL)
    {
      handle = dlopen("libcrypto.so", RTLD_LAZY);
      if (!handle)
        {
	  char* fn = xalt_dir("lib64/libcrypto.so");
	  handle = dlopen(fn, RTLD_LAZY);
	  if (fn)
            {
              memset(fn, '\0', strlen(fn));
              my_free(fn);
            }
          if (!handle) 
            {
              fputs(dlerror(), stderr);
              memset(hash, '\0', SHA_DIGEST_LENGTH);
              return;
            }
        }
      *(void **)(&sha1) = dlsym(handle, "SHA1");
      if ((error = dlerror()) != NULL) 
        {
          fputs(error, stderr);
          memset(hash, '\0', SHA_DIGEST_LENGTH);
          return;
        }
    }

  (*sha1)(buffer, fileSz, hash);
  if (munmap(buffer, fileSz) == -1) 
    perror("Error un-mmapping the file");

  close(fd);

  for (i = 0; i < 20; i++)
    sprintf(&sha1buf[i*2], "%02x", hash[i]);
}

void compute_sha1_cleanup()
{
  if (handle)
    dlclose(handle);
}
