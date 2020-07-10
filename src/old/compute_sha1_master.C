#include "xalt_config.h"
#include "compute_sha1.h"
#include "compute_sha1_master.h"
#include "xalt_dir.h"
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

pthread_mutex_t mutex;
long            fnSzG;
int             iworkG = -1;  

void* do_work(void *t)
{
  while(1)
    {
      pthread_mutex_lock(&mutex);
      iworkG++;
      pthread_mutex_unlock(&mutex);
      if (iworkG >= fnSzG)
        break;
      compute_sha1(argV[iworkG].fn.c_str(), &argV[iworkG].sha1[0]);
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
