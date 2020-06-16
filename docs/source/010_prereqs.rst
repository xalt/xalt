Prerequisite: Lua and Lmod
--------------------------

Lua and Lmod is required to associate modulefile with the tracked
library, and to track the external functions (subroutines) resolved to
the library.  Lmod is a modern Environment Module System.  If you have
Lmod installed you can bypass this step and go to XXX.  If you do not
wish to map directories to modules you can also bypass this step.

If you already have a module installed that is different from Lmod you
will still want to installed Lmod.  Your users can continue to use
your current module system. Lmod will just be used to build the
reverse map. Lmod can process TCL based modulefiles, so there is no
need to translate any TCL modulefiles into Lua.

Lmod make another aspect of using XALT easier.  XALT uses a wrapper
script *ld* as part of the linking phase of building an application.
This wrapper is used to extract build information as well as to encode
useful in the executable. Many modern compiler include *ld* as part of
the compiler.  To use the linker wrapper, XALT requires that the PATH
be order so that XALT's *ld* is found before the real *ld*.  Lmod
makes this an easier operation.  A site that uses other environment module
systems will require placing copies or links of XALT's *ld* in path
appropriate locations for each installed compiler. And updated
correctly each time a compiler is installed.

XALT can work correctly without the ld wrapper being used, it is just
that the site won't know things like which user built the binary or if
it was build by C, C++, or Fortran or some other compiler.


Installing Lua and Lmod
^^^^^^^^^^^^^^^^^^^^^^^

See https://lmod.readthedocs.io/en/latest/030_installing.html for how
to install Lua and Lmod.

