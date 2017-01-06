#include <fcntl.h>
#include <iostream>
#include <openssl/sha.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "run_submission.h"
#include "capture.h"
#include "xalt_config.h"
#include "epoch.h"

struct Arg
{
  Arg(std::string& fnIn)
    : fn(fnIn), sha1('') {}
  std::string fn;
  std::string sha1;
};

pthread_mutex_t mutex;
ArgV            argV;
int             iworkG = -1;  
int             fnSzG  =  0;

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

void parseLDD(std::string& exec, std::vector<Libpair>& libA, double& t_ldd, double& t_sha1)
{
  std::string  cmd;
  Vstring      result;

  // Capture the result from running ldd on the executable
  cmd  = LDD " " + exec + " 2> /dev/null";
  double t1 = epoch();
  capture(cmd, result);
  t_ldd = epoch() - t1;

  /*
   * The result of ldd look something like this:
   *    linux-vdso.so.1 =>  (0x00007ffcca6c1000)
   *    libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00002baa392c8000)
   *    libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00002baa395cc000)
   *    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00002baa397e2000)
   *    libuuid.so.1 => /lib/x86_64-linux-gnu/libuuid.so.1 (0x00002baa39ba7000)
   *    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00002baa39dac000)
   *    /lib64/ld-linux-x86-64.so.2 (0x00002baa390a3000)
   *
   *  We are only keeping the lines that match the pattern:
   *    libuuid.so.1 => /lib/x86_64-linux-gnu/libuuid.so.1 (0x00002baa39ba7000)
   */

  t1 = epoch();
  std::string::size_type s1,s2;
  std::string            lib, sha1;
    
  int sz = result.size();
  for (int i = 0; i < sz; ++i)
    {
      std::string& s = result[i];
      s1 = s.find("=> /");
      if (s1 == std::string::npos)
        continue;
      s2  = s.find(" (",s1);
      lib = s.substr(s1+3, s2-(s1+3));
      argV.push_back(Arg(lib))
    }
  fnSzG = argV.size();

  int            nthreads = std::min(std::min(sysconf(_SC_NPROCESSORS_ONLN), fnSzG),16);
  pthread_t*     threads  = new pthread_t[nthreads];
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init(&mutex, NULL);
      
  for (ithread = 0; ithread < nthreads; ++ithread)
    {
      int rc = pthread_create(&threads[ithread], &attr, do_work,
				  (void *) &ithread);
      if (rc)
        {
          printf("Error: pthread_create return code: %d\n",rc);
          exit(-1);
        }
    }

  for (ithread = 0; ithread < nthreads; ++ithread)
    {
      int rc = pthread_join(threads[ithread], NULL);
      if (rc)
        {
          printf("Error: fileNo: %d pthread_join return code: %d\n",i, rc);
          exit(-1);
        }
    }
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&mutex);
  for (int i = 0; i < fnSzG; ++i)
    {
      Libpair libpair(argV[i].fn, argV[i].sha1);
      libA.push_back(libpair);
    }
  delete [] threads;
  t_sha1 = epoch() - t1;
}
