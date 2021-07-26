Creating a config.py file for your site
---------------------------------------

XALT requires a configuration file written in python.  There are
examples in the Config directory.  In particular, the file
Config/TACC_config.py is the configuration we use at TACC.  This file
defines three types of filtering.  One is hostname filtering.  It is
typical to only track program execution on compute nodes.::

   **Note** XALT tracks linking of executables on both login and compute nodes.

The second type of filtering is by executables path. Finally there is
filtering of the environment variables.  It turns out that storing the
environment is expensive in terms of space in the database.  It is
important to chose a very limited set of variables to save, on the
order of 5 to 10 variables per execution.  For details about this file
please read over the Config/TACC_config.py file for details.  Also pay
attention to the commas.

For testing purposes, you might consider using the
Config/rtm_config.py as your model for your config.py file and then
switch to converting the TACC_config.py file for production.

XALT supports scalar (non-MPI) and MPI program sampling based on execution
time.  If you have setup XALT to track scalar or PKGS programs then
you can also control what percentage will be sampled.  Note that the
environment variable:

    XALT_SAMPLING=yes

must be set to "yes" in order for sampling to occur.  If this variable
is unset or is not set to "yes" then all program execution are
tracked.  This designed this way to make testing easier.

XALT also provides tracking of certain scalar programs (assuming your
config.py file turns this option on) that can track the internal
package use.  These programs could be python, R and MATLAB.  For these
programs a start record must be produced.  By default XALT will track
100% of these programs, but this might be overwhelming so XALT allows
a site to specify a percentage of these jobs to be tracked.  See
Config/TACC_config.py for more details.

XALT can capture every executable run by a user.  This is usually to
much data so XALT allows sites to control via a python configuration
file (called config.py from now on).  Sites should probably copy
Config/TACC_config.py to another name (Say SITE_config.py).  This
config.py file is only used when building and installing XALT.  It is
used to build (f)lex file which are converted to C code which is
compiled and linked into XALT for speed.

In other words, the config.py is **only used to build!**  It is
ignored by running programs on your system.


Tracking execution based on hostname
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In SITE_config.py you should change the hostname pattern to match your
naming scheme for your compute nodes.  Typically XALT only tracks
executables on the compute nodes and not the login nodes but if you
want to capture executions on all node you can do::

   hostname_patterns = [
     ['KEEP', '.*']  # capture executables on all nodes!
     ]

To capture just compute nodes then you'll have to match your scheme.
Suppose your compute nodes are named xyzNNN.site.org where N is a
digit then you could use::

   hostname_patterns = [
     ['KEEP', '^xyz[0-9][0-9][0-9]\..*']
     ]

It is also possible to use a C function which return 1 if XALT should
track and 0 if it should not. The template for this function is::

    #include <xalt_obfuscate.h>

    int my_hostname_parser(const char *hostname)
    {
      /* Return 1 to keep and 0 to reject */
      ...
    }

    void my_hostname_parser_cleanup()
    {
      /* Any cleanup required that my_hostname_parser requires */
      ...
    }   

The routines must be called my_hostname_parser() and
my_hostname_parser_cleanup().  You can provide a single file that
contains all the code you need as follows::

   --with-hostnameParser=c_file:parser.c

Or if you may build a shared library and tell XALT by::
  
   --with-hostnameParser=library:lib_parser.so

or if you allow for 32 bit applications to run then you need to build
the library for both 64 and 32 bits.::

   --with-hostnameParser=library:lib_parser.so:lib_parser_32.so

Tracking execution based on path
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sites will also want to control which executables are not tracked.
For executables there are three kinds: KEEP, SKIP and PKGS.  For
example lines like::

     path_patterns = [
          ['PKGS',  r'.*\/R'],
          ['PKGS',  r'.*\/MATLAB'],
          ['PKGS',  r'.*\/python[0-9.]*'],
          ...
          ['KEEP',  r'^\/bin\/cp'],
          ['KEEP',  r'^\/bin\/mv'],
          ['KEEP',  r'^\/bin\/gawk'],
          ['KEEP',  r'^\/bin\/sed'],
          ['KEEP',  r'^\/usr\/bin\/cp'],
          ['KEEP',  r'^\/usr\/bin\/mv'],
          ['KEEP',  r'^\/usr\/bin\/gawk'],
          ['KEEP',  r'^\/usr\/bin\/sed'],
          ...
          ['SKIP',  r'^\/bin\/.*'],
          ['SKIP',  r'^\/usr\/bin\/.*'],
          ...
          ['SKIP',  r'^\/opt/apps/intel\/.*'],
          ['SKIP',  r'^\/opt/apps/gcc\/.*'],
          ['SKIP',  r'^\/opt\/apps\/intel[0-9][0-9_]*\/mvapich2\/.*'],
          ['SKIP',  r'^\/opt\/apps\/intel[0-9][0-9_]*\/impi\/.*'],
          ['SKIP',  r'^\/opt\/apps\/gcc[0-9][0-9_]*\/mvapich2\/.*'],
          ...
          ['SKIP',  r'.*\/xalt\/.*'],
          ['SKIP',  r'.*\/conftest'],
          ['SKIP',  r'.*\/CMakeTmp\/cmTryCompileExec[0-9][0-9]*'],
          ['SKIP',  r'.*\/CMakeTmp\/cmTC_[a-f0-9][a-f0-9]*'],
     ]

You should look over the list provided by Config/TACC_config.py to
cover all the various types of executables listed there.  The above
list is an example of the kinds of executable classifications you will
want at your site.

First a comment about the regular expressions above. The python
program (SITE_config.py) is converted to a flex program which is then
converted to C and then compiled and included with the XALT shared
library. In flex the '/' is a regular expression character so it must
be quoted with a backslash. Also the matches are first come first
served.  That is when there is a match none of the pattern are checked
below.

For this reason, you will want to list the PKGS programs first (if
you have any), followed by the **KEEP's** and conclude with the
**SKIP's**

XALT allows for special treatment of programs such as R, Python and
MATLAB. With special hooks for each language/program, it is possible
to record the "packages" that each program uses. Those hooks record
package use during program execution which is written to a directory
in /tmp.  When the end record is produced, XALT checks to see if any
packages are recorded for the PKGS type executable.

The strategy that TACC uses is to keep program like cp, perl, gawk and
ignore all other system executables that are in /bin/, /usr/bin
etc. That is why, for example, **/bin/sed** and **/usr/bin/sed** are
marked as KEEP's where as **/bin** and **/usr/bin** are marked as
SKIP's.  That way /bin/ls is ignored but /bin/sed is tracked.  Also we
ignore all the compiler programs that live in the compiler
directories.  Similarly for the mpi helper program such as mpiexec
that live in the mpi directories.

We do not wish to track the generated programs from autoconf
(e.g. conftest) and Cmake (.*/CMakeTmp\/cmTryCompileExec[0-9][0-9]*).

Sampling Non-MPI executables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The SITE_config.py file also allows a site to control sampling of
scalar executables (including PKGS programs like R, MATLAB and
Python).  The details on how to control the sampling.  TACC uses the
follow scheme::

    0    - 1800 seconds (1 in 10000 chance of being recorded)
    1800 - 7200 seconds (1 in 100   chance of being recorded)
    7200 -  inf seconds (1 in 1     chance of being recorded)


This is expressed in your SITE_config.py file as::

    interval_array = [
        [    0.0,             0.0001 ],
        [ 1800.0,             0.01   ],
        [ 7200.0,             1.0    ],
        [ sys.float_info.max, 1.0    ]
    ]   

Sampling MPI executables
^^^^^^^^^^^^^^^^^^^^^^^^

MPI program execution presents an issue when it come to
tracking. Scalar (non-MPI) executions only produce an end record.
This means that if a scalar program abort before termination and is
not caught by the XALT signal handler, there is no XALT record
generated.

XALT is more interested in long running program, especially MPI
ones. Many long running MPI programs do not terminate, they use the
time-out feature of the local scheduler such as SLURM to end the job
after the maximum time in the job. XALT wants to capture these long
running MPI executions.  However, it is quite possible to overload the
machine keeping the XALT database with frequent short time, lower task
count MPI executions. So XALT allows MPI executions to be sampled. But
XALT has another setting, called MPI_ALWAYS_RECORD.  TACC sets this to 128.
This means that MPI executions are sampled if they have 127 tasks
or fewer are sampled.  

Sampling also means that there is no start record. For MPI executions
that are 128 and bigger do produce a start record but there is no
sampling.  That is all MPI executions which have 128 tasks or greater
are always recorded.  This also means that MPI execution which have
127 or fewer tasks than do not terminate normally will have no chance
of a record.

TACC uses the following sampling table::

    mpi_interval_array = [
        [    0.0,              0.0001 ],
        [  900.0,              0.01   ],
        [ 1800.0,              1.0    ],
        [ sys.float_info.max,  1.0    ]
    ]   

Controlling the Environment Variables collected
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You also get to control the user's environment that gets stored.  This
information can get quite large so it is important that you limit the
size of the environment that you record.  It is recommended that you
store no more that 10 environment variables per job on average.  So
only pick important variables.

Pre-ingestion Filtering
^^^^^^^^^^^^^^^^^^^^^^^

XALT version 2.10.26+ now support pre-ingestion filtering.  What this
means that if your site has a large number of json records and wants
to reduce the number of entries in the database, then your site's
Config.py file has another array to be filled out.  Below shows an
example filtering::

    pre_ingest_patterns = [
    #   percent   path pattern
        [0.0,     r'.*\/foobar'],
        [0.01,    r'.*\/BAZ'],
    ]   

Each row provides a number and a pattern.  Just like all the previous
patterns, if a pattern matches then it is used and no subsequent
patterns are used. The number is a value between 0 and 1.  A value of
0 states that there is zero percent chance of storing that pattern in
the database and 1 represents a 100 % chance of being recorded in the
database. A value of 0.01 as show for an executable named BAZ will
have a 1 % chance of being stored in the database.

There is already a default pattern automatically given last as which
states that::

    [1.0,    r'.*']

Namely that all remaining executables will be ingested into the
database.

The patterns are converted into a lex file which is converted into a C
routine which is then compiled into a shared library. This library is
used by the **xalt_file_to_db.py** and **xalt_syslog_to_db.py**
programs to control which json records are ingested into the database.
This means that any changes to **pre_ingest_patterns** must be
compiled and install on the VM or wherever your sites ingests json
records into the MySQL database.  It doesn't require re-installing
XALT on the compute nodes on your cluster.
