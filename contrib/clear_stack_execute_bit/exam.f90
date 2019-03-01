     program hello
     include 'mpif.h'
     real*4 f(3)
     integer rank, size, ierror, tag, status(MPI_STATUS_SIZE),n

     f(1) = 1.0
     f(2) = 2.0
     f(3) = 3.0
     n = 10

     call MPI_INIT(ierror)
     call MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierror)
     call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)
     write (*,10) f
     call MPI_FINALIZE(ierror)

  10 FORMAT(3F<N>.1)
     end
