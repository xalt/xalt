Building a reverse map for your cluster
---------------------------------------

The reverse map associates libraries  and objectfiles paths with
modulefile that specifies them. Libmap lists the  library files (.a
and .so) whose call to their function will be tracked by 
XALT. ReverseMap and Libmap are stored in the same file.::

    $ $LMOD_DIR/spider -o jsonReverseMapT $MODULEPATH > jsonReverseMapT.json

On a Cray (XC, XE, XK), this is a bit more involved, so a script has been
provided under contrib/ directory in XALT source distribution to make this
easier. One can simply use this script and run::

   $ xalt/contrib/build_reverseMapT_cray/cray_build_rmapT.sh $XALT_ETC_DIR

where $XALT_ETC_DIR where you would like to store these files for
ingestion into the database. 

How this works
^^^^^^^^^^^^^^
The Lmod spider command walks the directories found in $MODULEPATH and
finds all the modulefiles, including the ones that are part of the
software hierarchy if one exists (See https://lmod.readthedocs.io for
more details).

It finds changes to $PATH and $LD_LIBRARY_PATH in each modulefile to
know what paths are part of each module.  It also looks for the
setting of an environment variable of the form::

    <SITE>_<PkgName>_DIR

where <SITE> is the name of your site.  You have to tell Lmod that
your site is with the **LMOD_SITE_NAME** environment variable.  For
example the XYZ/3.2 package might be installed in /apps/xyz/3.2 and
the name of your site is BLUE then each module should have a line
similar to this::

    setenv("BLUE_XYZ_DIR", "/apps/xyz/3.2")


