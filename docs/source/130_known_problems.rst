Known problems with XALT
------------------------

Here is a list of known areas of concern.

Signal Handling
^^^^^^^^^^^^^^^

Some executable do not like the way XALT registers signals.  However
no reproducers have been reported.  To test if XALT's signal handler
presents a problem is to set the env. variable **XALT_SIGNAL_HANDLER**
to "no".  Note that it is optional to have XALT register a signal
handler.

Segfaults due to user's bad memory management
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

One reported case where a program worked without XALT and segfaulted
under XALT was due to bad memory management in the user's code. In
this particular case, the user code was using a link list.  The logic
in the code required that the next pointer in the link list struct was
NULL terminated at the end of the link list. However in this user
code, the step to NULL terminate the tail list was omitted.

Without XALT, the program gets the allocated memory with zero'ed
because every program starts that way.  With XALT, memory is used and
free'd which might have non-zero entries.

The fix for this particular program was to fix the link list
implementation. 

To minimize this risk, XALT uses *memset()* to clear the memory before
calling *free()*.  However, XALT can't control what system routines
such as *asprintf()* does with the memory they use internally.

A second case was reported where a code did something like the
following::

    #ifndef _GNU_SOURCE
    #define _GNU_SOURCE
    #endif

    #include <unistd.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>	

    extern char **environ;

    int my_copyenv(char *oldenv[])
    {
       char* eq;
       int   i;

       // The clearenv() function clears the environment of all
       // name-value pairs and sets the value of the external variable
       // environ to NULL. 

       clearenv();
       for (i = 0; oldenv[i]; i++) {
         if (!(eq = strchr(oldenv[i], '=')))
           continue;

         *eq = '\0';
         setenv(oldenv[i], eq + 1, 1);
       }
    }

    int main(int argc, char *argv[])
    {
       my_copyenv(environ);
    }
    
The problem with this code is that clearenv() is possibly freeing
memory that comes from **environ** then it walks the freed memory
via the **oldenv** variable.  This program doesn't normally seg-fault
but it can with XALT.  Because this program is walking freed memory it
is an unsafe program.

Segfaults due to name collision
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When XALT is being used,  the names used in the libxalt.so are in the
same namespace as the original program.  In one particular case, a
fortan program was build that provided its own *random()* routine
which was different from the *random()* routine provide by the c
library. XALT requires a UUID library which needs to call the
*random()* function.  The user's own *random()* was used by uuid
library rather than the c library one causing the program to
segfault.

You can detect this kind of problem if the program aborts in the
*myinit()* routine.

Earlier versions of XALT include libuuid routines as part of the
user executable.  Later versions have moved these calls to the
*xalt_run_submission* program so this particular name collision has
been removed.

This name collision is a potential problem in any executable.  XALT
uses macros to rename all names under its control to names that is
exceedingly unlikely to be found in user code (see
src/xalt_obfuscate.h).  However XALT cannot control user codes,
particular FORTRAN codes, that reuse names of routines in the C
library that XALT depends on. Normally, FORTRAN programs do not
present a problem because the subroutine or function ABC is usually
linked in with the name *abc_*. But the fortran compilers can
optionally told to not add the trailing underscore. By removing the
trailing underscore, FORTRAN routines can collide with the names of C
routines including routines in the standard C library.

C or C++ programs are unlikely to reuse names from the C library but
this can be a problem for FORTRAN programs which turn off the trailing
underscore in the link name.

This issue appears to be rare as only one case has been reported.
