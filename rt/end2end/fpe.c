#include <stdio.h>
#include "mpi.h"
int main(int argc, char* argv[])
{
  int ierr, myProc, nProc;

  ierr = MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myProc);
  MPI_Comm_size(MPI_COMM_WORLD, &nProc);

  if (myProc == 0)
    {
      int x, y;
      y = 0;
      x = 1/y;
      y = x;
      printf("x: %d, y: %d\n",x,y);
    }
  MPI_Finalize();
}
