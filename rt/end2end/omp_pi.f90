! *****************************************************************************
!
!  main.f95 -- Compute pi via integration -- OpenMP test program
!
!  Doug James
!  19 Jan 2012
!
! *****************************************************************************

program main

    use omp_lib
    implicit none

    ! -----------------------------------------------------

    integer(4)          :: totalsubints
      ! total number of sub-intervals in the integration
    integer(4)          :: totalthreads
      ! total threads used in computation
    integer(4)          :: subint
      ! loop counter for the sub-intervals

    real(8)             :: computedpi
      ! the value of pi as computed by this program
    real(8)             :: subwidth
      ! width of each sub-interval
    real(8)             :: halfwidth
      ! half of the width of each sub-interval
    real(8)             :: x, y
      ! x and y coordinates of a point on the graph of y = f(x)
    real(8)             :: totaltime
      ! total execution time in seconds
    real(8)             :: starttime, stoptime
      ! start and stop time in seconds

    real(8)                   :: ACTUAL_PI

    integer(4), parameter     :: STDOUT_UNIT   =  6

    ! -----------------------------------------------------
    ! Specify number of integration 

    totalsubints = 2000000

    ACTUAL_PI = 4.0_8*atan(1.0_8)


    ! Do computation...

    starttime = 0.0_8            ! no timing info reported if omp turned off
!$  starttime = omp_get_wtime()  ! treated as comment if omp turned off

    totalthreads =  0
!$  totalthreads = omp_get_max_threads()

    subwidth = 1.0_8 / totalsubints
    halfwidth = 0.5d0 * subwidth              ! 0.5d0 is "old school" synonym for 0.5_8

    computedpi = 0.0_8

    !$omp parallel do    private( x, y )     reduction(  +: computedpi )

        do subint=0, ( totalsubints - 1 )
            x = subwidth*subint + halfwidth
            y = f(x)
            computedpi = computedpi + y
        end do

    !$omp end parallel do

    computedpi = computedpi * subwidth

    stoptime  = 0.0_8            ! no timing info reported if omp turned off
!$  stoptime  = omp_get_wtime()  ! treated as comment if omp turned off
    totaltime = stoptime - starttime

    ! Report results...

    call reportresults(                                                       &
      &  STDOUT_UNIT,  computedpi, totalsubints, totalthreads, totaltime )

! ---------------------------------------------------------
!Internal sub-programs...

contains

    subroutine reportresults(                                                 &
      &  fileunit, computedpi, totalsubints, totalthreads, totaltime )

        integer(4), intent(in)   :: fileunit
        real(8),    intent(in)   :: computedpi
        integer(4), intent(in)   :: totalsubints
        integer(4), intent(in)   :: totalthreads
        real(8),    intent(in)   :: totaltime

        ! ---------------------------------------

        real(8)             :: abserror
          ! absolute error in calculation
        real(8)             :: relerror
          ! relative error in calculation

        write( fileunit, * )
        write( fileunit, * ) '  ************************************* '
        write( fileunit, * ) '     OpenMP Test Program: Compute Pi    '
        write( fileunit, * ) '  ************************************* '
        write( fileunit, * )

        write( fileunit, 100 ) "Computed Value:", computedpi
        write( fileunit, 100 ) "Actual Value:",   ACTUAL_PI
 100    format( 5x, a25, 1x, f19.16 )

        abserror = abs( computedpi - ACTUAL_PI )
        relerror = abserror / abs( ACTUAL_PI )

        write( fileunit, 200 ) "Absolute Error:", abserror
        write( fileunit, 200 ) "Relative Error:", relerror
 200    format( 5x, a25, 1x, 1pe8.1 )

        write( fileunit, 300 ) "Total sub-intervals:", totalsubints
 300    format( 5x, a25, 1x, i10 )
        write( fileunit, 400 ) "Total threads:", totalthreads
 400    format( 5x, a25, 1x, i3 )

        write( fileunit, 500 ) "Total time:", totaltime
 500    format( 5x, a25, 1x, 1pe8.1, " secs" )

        write( fileunit, * )
        write( fileunit, * ) '  ------------------------------------- '
        write( fileunit, * )

    end subroutine reportresults

    ! -------------------------------------------

    real(8) function f(x)
        real(8), intent(in)  ::  x
        f = 4.0_8 / ( 1.0_8 + x*x )
    end function f



end program main
