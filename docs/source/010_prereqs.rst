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

Installing Lua and Lmod
^^^^^^^^^^^^^^^^^^^^^^^

See https://lmod.readthedocs.io/en/latest/030_installing.html for how
to install Lua and Lmod.

