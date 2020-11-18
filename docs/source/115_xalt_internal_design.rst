.. _internalDesign-label:

XALT's Internal Design
----------------------

As the overview stated, XALT works in two parts: It puts an *ld*
wrapper shell script which add a *watermark* to every executable built
under XALT.  This wrapper script also creates a \*.json record about
the link.

The second part relies on a feature of the ELF binary format used by
Linux by providing a way for routines to be executed before *main* and
after *main* completes.

Instrumenting Executables via the linker
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is the responsibility of the site installers to make sure that
XALT's *ld*, *ld.gold* and *x86_64-linux-gnu-ld* wrapper scripts are
found before the compilers' or system's *ld* program (See
:ref:`xalt_modulefile-label`).  This means that XALT's *ld* is earlier
in the path and that **COMPILER_PATH** env. variable includes the
location of XALT's *ld*.

Typically the compiler calls the linker to build the executable. So
the compiler calls XALT's *ld* or similar scripts with add the
watermark and captures the shared libraries that are part of the
build.

This information about the build is encoded in a \*.json file that is
typically transmitted to the database.

Once the wrapper script is finished its work, the true linker is
called to build the executable.

Possibly calling the true linker multiple times 
===============================================

If xalt function tracking is enabled then the linker is
called twice.  Obviously, if your site is not interested in XALT
function tracking then this can be disabled.  Note also that XALT
function tracking means that a function is in the executable.  There
is no run-time tracking.

The true linker will be called another time if any of the follow
options are passed to the linker:

#. -M, --print-map
#. --print-output-format
#. --print-memory-usage

Use of these options is rare.

Running Executables not build under XALT
========================================

The advantage to building an executable is that the executable has a
watermark.  However XALT can track all dynamically build executables
without the watermark. 

XALT can track statically built executables when built under XALT.
Statically built executables outside of XALT cannot be tracked. 


Tracking Executable with XALT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

At the core of XALT are the routines in the src/xalt_initialize.c
file. There are two routine *myinit()* and *myfini()*. There is
nothing special about those name.  The trick is that the *myinit()*
routine is added to the *.init_array* as part of the ELF binary format
standard. Any routine added to that array is called before *main()* is
run. Note that this is true for FORTRAN executables as well.

The *myfini()* routine is added to the *.fini_array*.  Any routine
that is added to that array is run after *main()* completes.  If
*main()* terminates early then *myfini()* is not called, unless caught
by the signal handler registered by XALT in *myinit()* (note this is
normally disabled).

Setting the LD_PRELOAD environment variable to point to the XALT
shared library means that every dynamic executable calls the
*myinit()* routine.  XALT makes every effort decide quickly if
to track or not. Here are the series of questions that an executable
must pass to be tracked in the *myinit()* routine:

0. XALT can have two copies of *myinit()* through the link process and
   via using **LD_PRELOAD**.  Using environment variable, XALT
   prevents this from happening more than once.

#. It checks that the env. variable **XALT_EXECUTABLE_TRACKING**
   equals the string "yes".

#. Only non-MPI executable or task 0 MPI executables are tracked.

#. The hostname is check to be acceptable.

#. non-MPI executables require the env. var XALT_SCALAR_TRACKING to be yes.

#. MPI executables require the env. var XALT_MPI_TRACKING to be yes.

#. The full path to the executable is checked to see that it acceptable
   to the path parser controlled by the configuration file.


If any of the above test are negative then XALT tracking is avoided.
The next decision is whether a start record is generated. Since XALT
can generate a large number of records, it makes it easy to not
capture every execution. The configuration file also allows for
sampling.  To allow for sampling all non-MPI execution, no start
record is generated. Also small MPI execution (small determined by the
configuration file) no start record is generated.  This means that if
the program terminates early then no record is created.

Larger MPI executions by the number of tasks do produce a start
record.  This means that long running MPI that terminate by the job
scheduler, such as SLURM, will be recorded, without an end-time.

Producing an end record
=======================

If an execution still has XALT tracking turned on and it completes
*main()* then the *myfini()* routine will decide whether to produce an
end record. For non-MPI and small number of tasks MPI, the run-time of
the program is used to decide whether to produce the end record.
Based on the sampling rules in the configuration file, this time used
to compute a probability. If accepted then an end record is generated .
For large MPI programs an end record is always produced.


How an record is produced
=========================

When producing a record, either *myinit()* or *myfini()* routines do a
system call on the C++ XALT program called *xalt_run_submission* to
actually generate the \*.json record transmitted to the database.

This program reads its command line argument as well as interrogate
the environment and the /proc filesystem to collect the information
that the \*.json record contains.

Once the data is collected the routine *transmit()* is called to
somehow send the information encoded in json tables to be passed to
the database. This can be via file or syslog or curl.


Signal Handling
===============

As the above discussion has pointed out, the only way to produce an
end record is that the *myfini()* get called. Typically this means
that *main()* completes.  So if a program terminates early due to a
segfault or floating point error then no end record will be produced.

To deal with this the *myinit()* routine can register a signal handler,
*wrapper_for_myfini()* to be called if necessary.  Note that this only
happens this execution could be tracked. This wrapper routine
calls the *myfini()* routine, clears the XALT signal handler and
re-raises  the original signal.  This code can be found in the
src/xalt_initialize.c file at the end of the *myinit()* routine.
XALT uses the sigaction struct to register the handler.  Since
*myinit()* is called before main, it is likely that it does so first.
However it only registers a new handler if the old one is NULL.

This works for non-MPI program execution.  For MPI program execution
there are a some issues.  First, since XALT is only active on task 0,
a signal event on tasks other than zero are ignored.  Also job
schedulers such as SLURM do send a TERM signal when the job terminates
for time, this signal is not always passed through the MPI job runner
(i.e. mpiexec) to the MPI application. Since this is not reliable,
XALT will continue to produce start records for large number of tasks
for MPI jobs.

For the above reasons, XALT (as of version 2.10.2+) no longer
registers a signal handler.  To activate use the configure option
(--with-signalHandler=yes) or set the
env. var. XALT_SIGNAL_HANDLER=yes.

