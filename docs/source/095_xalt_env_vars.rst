XALT's environment variables
----------------------------

There are a few environment variables that must be set in order for
XALT to work correctly.


**XALT_EXECUTABLE_TRACKING** : This variable must be set to **yes**
   otherwise there is no XALT tracking

**XALT_DIR** : This directory is the base directory where XALT is
   installed on your system.  This is the directory that contains bin,
   include, lib, lib64, etc.

**LD_PRELOAD** : This path like variable contains the path to the
   libxalt_init.so file.

**XALT_SAMPLING** : This variable should be set to **yes** unless you
   are testing XALT.

**COMPILER_PATH** : For the gnu and intel compilers this needs to be
   set to the bin directory containing the **ld** program from XALT.

**XALT_SCALAR_TRACKING** : Normally configured to be **yes** it can be
  overridden by an environment variable. *yes* is on and anything else
  is off.

**XALT_MPI_TRACKING** : Normally configured to be **yes** it can be
  overridden by an environment variable. *yes* is on and anything else
  is off.

**XALT_GPU_TRACKING** : Can be configured to be **yes** it can be
  overridden by an environment variable. **yes** is on and anything else
  is off.

**XALT_SIGNAL_HANDLER** : Normally XALT turns on signal handler to
  catch segfaults, FPE, etc. Setting this variable to **no** disables
  this feature.


Debugging variables for XALT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**XALT_TRACING** : This variable allows for debugging the actions of
   XALT.

This variable can take several values:

*link* : This will tracing the linking steps

*yes* : This will tracing the execution of a program under XALT. This
   gives the most information.  This option will report whether an
   execution will be tracked or why not. 

*yes0* : This will tracing the execution of a program under XALT but
   only for task 0. This option will report whether an execution will
   be tracked or why not for task 0.

*run* : This shows output for execution that will be tracked. There is
   nothing printed when a program is not being tracked.


Special Debugging variables for XALT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**XALT_TESTING_RUNTIME** : If this variable is set, XALT uses it to be
   the runtime for the executable instead of the real
   runtime. Obviously this is useful when testing the sampling rules.


**XALT_MPI_ALWAYS_RECORD** : XALT will use this env. variable instead
   of the configured value.

**XALT_DUMP_ENV** : Setting this variable to **yes** will print the
  current environment to stderr.


Environment variable pushed by XALT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**XALT_RUN_UUID** : The current run uuid created by XALT.  This is
   used by programs such as python, R and MATLAB so that they know what
   executable has been called.  Program like python can track
   imports. This env. variable is part of that mechanism.

**XALT_DIR** : The location of the XALT programs

**XALT_DATE_TIME** : The current date and time of when the current
   executable was started.

