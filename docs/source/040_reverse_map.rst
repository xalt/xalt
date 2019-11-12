.. _reverseMap-label:

Building a reverse map for your cluster
---------------------------------------

The reverse map associates libraries  and objectfiles paths with
modulefile that specifies them. Libmap lists the  library files (.a
and .so) whose call to their function will be tracked by 
XALT. ReverseMap and Libmap are stored in the same file.::

    $ $LMOD_DIR/spider -o xalt_rmapT      $MODULEPATH > xalt_rmapT.json

On a Cray (XC, XE, XK), this is a bit more involved, so a script has been
provided under contrib/ directory in XALT source distribution to make this
easier. One can simply use this script and run::

    $ xalt/contrib/build_reverseMapT_cray/cray_build_rmapT.sh $XALT_ETC_DIR

where $XALT_ETC_DIR where you would like to store these files for
ingestion into the database. 

Function Tracking
^^^^^^^^^^^^^^^^^

If your site wishes track functions from system libraries then the 
You will need the *xalt_rmapT.json* to found on every node.  For
example TACC specifies at configuration time::

   --with-etcDir=/tmp/moduleData

Then uses Lmod to create the directory **/tmp/moduleData/reverseMapD**
and places a new copy of **xalt_rmapT.json**.

If Lmod is installed then the
**$LMOD_DIR/update_lmod_system_cache_files** can be used. For example,
to create the cache files to be found in **/tmp/moduleData** the
following command could be used::

   $LMOD_DIR/update_lmod_system_cache_files  -t /tmp/system.txt -R /tmp/moduleData/reverseMapD -J -K $BASE_MODULE_PATH

where **$BASE_MODULE_PATH** the system $MODULEPATH.  Note the creation
of the reverseMap file is only necessary if a site wishes to track
functions.  See lmod documentation (https://lmod.readthedocs.io) for
more details.  There is an advantage to use the
**update_lmod_system_cache_files** script over the **spider**
command. The first script knows how to update the reverseMap file that
can be done on a live system.

Matching executable and library paths to modules
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Independent of whether a site wishes to do function tracking, The
reverseMap file **xalt_rmapT.json** is also needed to map executable
and library paths to modules.  This is not required to use XALT, it is
just that the database won't know that a particular library or
executable came from a particular module.

If your site uses syslog to transfer the json records, then it is
likely that the **xalt_rmapT.json** file will need to be transfered to
the VM that ingested these records (See
:ref:`syslog_reverseMap-label`).


 

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
