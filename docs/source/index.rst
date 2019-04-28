XALT 2: Tracking user executables and library usage on your cluster
===================================================================

PURPOSE
^^^^^^^
XALT 2 is a tool to allow a site to track user executables and library
usage on a cluster.  When installed it can tell a site what are the
top executables by Node-Hours or by the number of users or the number
of times it is run.  XALT 2 also tracks library usage as well.  XALT 2
can also track package use by R, MATLAB or Python.  It tracks both MPI
and non-MPI programs.  

OVERVIEW
^^^^^^^^

XALT  does its tracking by ''hijacking'' the linker and by use
of the environment variable **LD_PRELOAD** to have code run before and
after main().  And yes it works with Fortran programs as well.  Since
XALT 2 could track every single program on your cluster, XALT 2 allows
for site controllable filtering.  Otherwise an data collection scheme
would get overwhelmed.

How XALT2 works
^^^^^^^^^^^^^^^

XALT2 works in two parts. It provides an *ld* wrapper shell script
which adds a *watermark* to every executable built with XALT2 as well
as create a \*.json record of the link for further analysis.

The second part is to measure the run-time and other information about
each executation that is tracked. It relies on a feature of the ELF
binary format that Linux uses. ELF supports an init array of function
pointers that if they have a value are run before main(). There is a
second array, called the fini array that is run after main()
completes if the program runs to completion, i.e. if it doesn't
segfault or otherwise terminate early.  Also ELF supports reading the
environment variable LD_PRELOAD.  If this variable points to a shared
libary (\*.so), then that share libary is also linked in to every
non-static executable.  A simple example C program, *try.c*::

     #include <stdio.h>
     int main()
     { printf("Hello World!\n"; return 0;}
  
can have its execution modified by a shared library that is built from
the following C code, *xalt.c*, that is compiled by a GCC compiler::

     #include <stdio.h>
     void myinit(int argc, char **argv)
     { printf("This is run before main()\n");}
     void myfini()
     { printf("This is run after main()\n");}

     __attribute__((section(".init_array"))) __typeof__(myinit) *__init = myinit;
     __attribute__((section(".fini_array"))) __typeof__(myfini) *__fini = myfini;

If the *try.c* program is built and run, its output would be::

    $ gcc -o try try.c; ./try
    Hello World!

But if *xalt.c* program is built into a share library called
libxalt.so and is in the same directory with the *try* executable,
then::

    $ gcc -c -fPIC xalt.c; gcc -o libxalt.so -fPIC -shared xalt.o
    $ LD_PRELOAD=./libxalt.so ./try
    This is run before main()
    Hello World!
    This is run after main()

This is all **without** making a single change to the ./try
executable.

The actual shared library that XALT builds generates a \*.json file
that contains the run-time, the number of task and other information
like the shared libraries linked in with each executable that is tracked.
Note that XALT is easily site configured to only track executables
that only run on the compute nodes or have certain paths or other
criteria to avoid tracking every single program that is run on a
system. 

Even those the above example was with a C program, this trick works with C++
and Fortran programs as well.  It works with any ELF binary that is
dynamically linked. It does not work with a static binary.


Installing XALT2
^^^^^^^^^^^^^^^^

Installing XALT 2 is both straightforward and complicated.  It is
straightforward in that it only requires a configure step and **make
install**.  But it is complicated because a site will be required to
taylor the filtering to meet their site's needs.  The install steps
are divided into the following steps.

.. toctree::
   :maxdepth: 1

   010_prereqs
   020_site_configuration
   030_site_filtering
   040_install_and_test
   050_reverse_map
   060_setup_db
   070_loading_json_by_file
   080_loading_json_by_syslog

Advanced Topics
^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   090_execstack

FAQ
^^^

Here we show answers to Frequently Asked Questions.

.. toctree::
   :maxdepth: 1

   999_faq
   
