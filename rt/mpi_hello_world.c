#include <stdio.h>
#include "mpi.h"
int main(int argc, char* argv[])
{
  
  int ierr, myProc, nProc;

  ierr = MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myProc);
  MPI_Comm_size(MPI_COMM_WORLD, &nProc);

  if (myProc == 0)
    printf("Hello World from proc: %d out of %d !\n",myProc, nProc);

  MPI_Finalize();

  return 0;
}
