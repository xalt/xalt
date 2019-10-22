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

*ld* wrapper script
^^^^^^^^^^^^^^^^^^^

It is the responsibility of the site installers to make sure that
XALT's *ld* wrapper script is found before the compilers' or system's
*ld* program (See :ref:`xalt_modulefile-label`).  This means that
XALT's *ld* is earlier in the path and that **COMPILER_PATH**
env. variable includes the location of XALT's *ld*.  

Static Binaries Tracking
^^^^^^^^^^^^^^^^^^^^^^^^

At the core of XALT is the routines in the src/xalt_initialize.c
file. It contains
