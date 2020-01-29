#include "xalt_config.h"
#include "compute_sha1.h"
#include <fcntl.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>

static void (*sha1)(const unsigned char *d, size_t n, unsigned char *md) = NULL;

pthread_mutex_t mutex;
long            fnSzG;
int             iworkG = -1;  

void compute_sha1(std::string& fn, std::string& sha1_str)
{
  struct stat    st;
  int            fd, i;
  char *         error;
  char           sha1buf[41];
  unsigned long  fileSz;
  unsigned char* buffer;
  unsigned char  hash[SHA_DIGEST_LENGTH];
  
  fd     = open(fn.c_str(), O_RDONLY);
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
      void * handle = dlopen("libcrypto.so", RTLD_LAZY);
      if (!handle)
        {
          handle = dlopen (xalt_dir("lib64/libcrypto.so"), RTLD_LAZY);
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
  sha1_str.assign(sha1buf);
}

void* do_work(void *t)
{
  while(1)
    {
      pthread_mutex_lock(&mutex);
      iworkG++;
      pthread_mutex_unlock(&mutex);
      if (iworkG >= fnSzG)
        break;
      compute_sha1(argV[iworkG].fn, argV[iworkG].sha1);
    }
  pthread_exit(NULL);
}

void compute_sha1_master(long n)
{
  fnSzG          = n;

  // Only compute SHA1 sum if XALT_COMPUTE_SHA is yes
  // If not computing it then set result to "0"
  const char * v = getenv("XALT_COMPUTE_SHA1");
  if (v == NULL)
    v = XALT_COMPUTE_SHA1;
  if (strcmp(v,"yes") != 0)
    {
      for (long i = 0; i < fnSzG; ++i)
        argV[i].sha1 = "0";
      return;
    }
                              
  // If we are here then compute sha1 sum.

  long           nthreads = std::min(std::min(sysconf(_SC_NPROCESSORS_ONLN), fnSzG),16L);
  pthread_t*     threads  = new pthread_t[nthreads];
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init(&mutex, NULL);
      
  for (long ithread = 0; ithread < nthreads; ++ithread)
    {
      int rc = pthread_create(&threads[ithread], &attr, do_work,
				  (void *) &ithread);
      if (rc)
        {
          printf("Error: pthread_create return code: %d\n",rc);
          exit(-1);
        }
    }

  for (long ithread = 0; ithread < nthreads; ++ithread)
    {
      int rc = pthread_join(threads[ithread], NULL);
      if (rc)
        {
          printf("Error: pthread_join return code: %d\n", rc);
          exit(-1);
        }
    }
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&mutex);
  delete [] threads;
}
