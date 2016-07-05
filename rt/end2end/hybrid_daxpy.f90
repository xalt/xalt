program bsaxpy
   include "mpif.h"
   integer,parameter :: N=256*8 , M=256
!  integer,parameter :: N=1024*40, M=1024
   real*8,save  :: a(n,m),b(n,m),c(n+13,m)

   call mpi_init(ierr)
   call mpi_comm_size(MPI_COMM_WORLD,npes, ierr)
   call mpi_comm_rank(MPI_COMM_WORLD,irank,ierr)

   call MPI_Barrier(MPI_COMM_WORLD,ierr)

   call suba(irank,N,M,a,b,c)

   call MPI_Finalize(ierr)
end program

subroutine suba(irank,N,M,a,b,c)
   real*8  :: a(n,m),b(n,m),c(n,m)
   real*8  :: s,time,rate, t

   a=1.1;b=1.1;s=2.0

   call system_clock(count_rate=irate)
   call system_clock(it0)
   call system_clock(it1)

   !$OMP PARALLEL  private(i,j), shared(a,b,c,N,M,s), private(k)

   do k = 1,8
     s = s + 1.0
     !!OMP DO  private(i,j)
     !$OMP DO  private(i,j), schedule(static)
     !!OMP DO  private(i,j), schedule(dynamic,64)
     !!OMP DO  private(i,j), schedule(runtime)
     do i = 1,M
       do j = 1,N
          c(j,i) = a(j,i)*s+b(j,i)
       end do
     end do
     t = t+c(N,M)+c(1,1)  !Keeps optimizer from removing outer loop
   end do
   
   !$OMP END PARALLEL

   call system_clock(it2)
   time = dble(it2 - it1*2 + it0)/dble(irate)
   rate = 100*dble(2*(M*N))/(dble(1024*1024)*time)
   if(irank==0) print*," MFLOPS=",rate,"    time=",time
   write(7) t   ! make sure t is used some place

end subroutine
