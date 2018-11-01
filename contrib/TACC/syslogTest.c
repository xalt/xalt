#include <stdio.h>
#include "mpi.h"
#include <syslog.h>

#include <sys/utsname.h>


int main(int argc, char* argv[])
{
  
  struct utsname myUname; 
  int ierr, myProc, nProc;

  ierr = MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myProc);
  MPI_Comm_size(MPI_COMM_WORLD, &nProc);


  uname(&myUname);



  setlogmask (LOG_UPTO (LOG_NOTICE));

  openlog ("syslogTest", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

  syslog (LOG_NOTICE, "Program started by User %d, from host: %s", getuid (), &myUname.nodename[0]);

  closelog ();

  MPI_Finalize();

  return 0;
}
