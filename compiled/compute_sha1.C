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


pthread_mutex_t mutex;
long            fnSzG;
int             iworkG = -1;  

void compute_sha1(std::string& fn, std::string& sha1)
{
  struct stat    st;
  int            fd, i;
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

  SHA1(buffer, fileSz, hash);
  if (munmap(buffer, fileSz) == -1) 
    perror("Error un-mmapping the file");

  close(fd);

  for (i = 0; i < 20; i++)
    sprintf(&sha1buf[i*2], "%02x", hash[i]);
  sha1.assign(sha1buf);
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
  fnSzG                   = n;
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
}
