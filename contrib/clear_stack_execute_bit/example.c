#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* sort an array, starting at address `base`,
 * containing `nmemb` members, separated by `size`,
 * comparing on the first `nbytes` only. */
void sort_bytes(void *base,  size_t nmemb, size_t size, size_t nbytes)
{
  int myCompare (const void * a, const void * b ) {
    const char *pa = *(const char**)a;
    const char *pb = *(const char**)b;

    return strcmp(pa,pb);
  }

    qsort(base, nmemb, size, myCompare);
}

int main(int argc, char* argv[])
{
  int i;
  int ierr, myProc, nProc;

  ierr = MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myProc);
  MPI_Comm_size(MPI_COMM_WORLD, &nProc);

  if (myProc == 0)
    printf("Hello World from proc: %d out of %d !\n",myProc, nProc);


  const char* a[] = { "abc", "111","def", "zzz","222" };

  sort_bytes(&a[0], 5, sizeof(char *), 3);

  if (myProc == 0)
    {
      for (i=0; i < 5 ; ++i)
        printf("%s\n", a[i]);
    }
  

  MPI_Finalize();
}   
