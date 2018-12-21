.. _execstack_bit-label:

XALT and the GNU Stack bit
^^^^^^^^^^^^^^^^^^^^^^^^^^

XALT adds a small bit of assembly code to all executables built
through the hijacked linker to record key value pairs such as the
**build_user** and **syshost** among several other values. The program
**xalt_extract_record** prints those values.

The GNU compilers want to know when the memory on the stack can be
executable.  It is not common for programs to need this however it is
possible.  The following program requires the GNU Stack bit set::

    #include <mpi.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    /* sort an array, starting at address `base`,
     * containing `nmemb` members, separated by `size`,
     * comparing on the first `nbytes` only. */
    void sort_bytes(void *base,  size_t nmemb, size_t size, size_t nbytes) {
        int compar(const void *a, const void *b) {
            return memcmp(a, b, nbytes);
        }
        qsort(base, nmemb, size, compar);
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

The compilers all handle setting this flag correctly.  However
assembly code is on its own.  Without a special header directive in
the assembly code, the linker decides that the GNU Stack bit must be
set. All version of XALT prior to 2.5 will have this GNU Stack bit
on all executables built with XALT's hijacked linker.  XALT version
2.5 and later have updated the generated assembly code cause this bit
not to be set.

Normally this is not a problem.  However, with Centos 7.5 on a machine
that any MPI program run on the latest OPA driver used with Intel MPI
will have a problem.  A typical error message would look like::

    c591-801.stampede2.tacc.utexas.edu.334447hfi_userinit: mmap of status page (dabbad0008030000) failed: Operation not permitted
    c591-801.stampede2.tacc.utexas.edu.334447hfp_gen1_context_open: hfi_userinit: failed, trying again (1/3)

The important part of the message is the hfi_userinit. Programs with
the bit set still work with MVAPICH2 and OpenMPI.

The migration strategy is to clear the bit using the execstack
program on machine that use Intel MPI and OPA network hardware.  There
are two tricks that XALT sites can use the database.  Since all linked
programs will be found in the database you can use the following
program to clear the bit in those executables::

   contrib/clear_stack_execute_bit/xalt_db_clear_bit_link.py

This will find those executables that have not been moved.  You also
know all the programs that have been run with that have been built
with XALT.  You can use the following program to clear those programs
as well::

   contrib/clear_stack_execute_bit/xalt_db_clear_bit_run.py

The program **contrib/clear_stack_execute_bit/monthly_clear_bits.sh**
was used by TACC on the Stampede2 cluster. For both clear bit
programs, the **xalt_extract_record** program must be in the PATH for
root.


