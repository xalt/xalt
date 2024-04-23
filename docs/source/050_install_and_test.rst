Running ./configure and building XALT
-------------------------------------

Installing XALT is different from most applications.  It is common
to have one version of XALT installed at a time. This is the safe way
to install XALT.  Namely use the "prefix" to the parent directory
above XALT.  In other word, if your wishes to install XALT in
/opt/xalt/<version>, then the configure line looks like::

    $ ./configure --prefix=/opt ...

and xalt will be installed in /opt/xalt/X.Y.Z where X.Y.Z the
current version of XALT.  This is the safest way to install XALT.


Site controlled XALT location
-----------------------------

A site may wish to control the location of XALT, especially if two or
more version of XALT will be installed at the same time.  In this case
a site might configure XALT by::

    $ ./configure --prefix=/opt/xalt/X.Y.Z --with-siteControlledPrefix=yes
  
Where X.Y.Z is the current version of XALT.

   **NOTE**: Site which to this must set the *XALT_DIR* environment variable. 

Please the next section if your site uses static binaries.


Sites using static executables
------------------------------

If a site builds static binaries AND has been using XALT with version
< 2.8, then there is a problem. Those static executables are expecting to
find XALT in a standard location. Typically if a site has been
installing XALT in /opt/xalt/X.Y.Z then xalt will install a symlink
which links /opt/xalt/xalt with /opt/xalt/X.Y.Z.

All static binaries built with XALT version prior to version 2.8 will
expect to find XALT tools in /opt/xalt/xalt/libexec/\*.  If
/opt/xalt/xalt/... doesn't exist then those old static binaries will
work but will silently not record data, unless the $XALT_TRACING=yes 

It is recommended that sites with static binaries maintain a symlink
between /opt/xalt/xalt (or wherever your site has installed XALT) for
the life of the machine.

Typical configure and build instructions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here is how we configure XALT for stampede2::

    $ cd xalt
    $ ./configure --prefix=/opt/apps              \
      --with-syshostConfig=nth_name:2             \
      --with-config=Config/TACC_config.py         \
      --with-transmission=syslog                  \
      --with-MySQL=no                             \
      --with-etcDir=/tmp/moduleData
    $ make install

If the current version of XALT is X.Y.Z then this will install XALT in
/opt/apps/xalt/X.Y.Z and /opt/apps/xalt/xalt will point to it.  Please
configure XALT to match your site's needs.


.. _createXALTdirs:

Creating XALT directories when using --with-xaltFilePrefix=...
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If your site is using the **file** transmission style and the
xaltFilePrefix option then, you must create the XALT directories as part of
the installation process.  Assuming that you have installed XALT with
a prefix of **/opt/apps**.  Then you can run as root::

    $ /opt/apps/xalt/xalt/sbin/create_xalt_directories.sh


.. _xalt_modulefile-label:

XALT modulefile
^^^^^^^^^^^^^^^

Below is the XALT modulefile you need to specify to use the
package.  The first one is a Lua modulefile that can be used with Lmod::


  setenv("XALT_EXECUTABLE_TRACKING",       "yes")

  -- Uncomment this to track GPU usage
  -- setenv("XALT_GPU_TRACKING",              "yes")

  local base  = "/opt/apps/xalt/xalt"  --> Change to match your site!!!
  local bin   = pathJoin(base,"bin")

  prepend_path{"PATH",          bin, priority="100"}
  prepend_path("XALT_DIR",      base)
  prepend_path("LD_PRELOAD",    pathJoin(base,"$LIB/libxalt_init.so"))
  prepend_path("COMPILER_PATH", bin)

  -- Uncomment these two lines to use XALT inside Singularity containers
  -- setenv("SINGULARITYENV_LD_PRELOAD", pathJoin(base,"$LIB/libxalt_init.so"))
  -- prepend_path("SINGULARITY_BINDPATH", base)

  -- Uncomment this line to have XALT track python packages.
  -- prepend_path("PYTHONPATH",    pathJoin(base,"site_packages"))

  ------------------------------------------------------------
  -- Only set this in production not for testing!!!
  setenv("XALT_SAMPLING",  "yes")  

The following is a TCL modulefile::

  setenv XALT_EXECUTABLE_TRACKING       yes

  # Uncomment this to track GPU usage
  #setenv XALT_GPU_TRACKING              yes

  ############################################################
  # Change /opt/apps/xalt to match your site!!
  ############################################################

  setenv        XALT_DIR        /opt/apps/xalt/xalt    
  prepend-path  PATH            /opt/apps/xalt/xalt/bin  100
  prepend-path  LD_PRELOAD      /opt/apps/xalt/xalt/\$LIB/libxalt_init.so
  prepend-path  COMPILER_PATH   /opt/apps/xalt/xalt/bin

  # Uncomment this to use XALT inside Singularity containers
  #setenv SINGULARITYENV_LD_PRELOAD /opt/apps/xalt/xalt/\$LIB/libxalt_init.so
  #prepend-path  SINGULARITY_BINDPATH   /opt/apps/xalt/xalt

  ############################################################
  ## Only set this is production not for testing!!!
  setenv XALT_SAMPLING  yes


Obviously, these modulefiles will need to be modified to match your
site's location of XALT.

  **Note**: To track GPU usage, XALT must also be configured with
  --with-trackGPU=yes.

  **Note**: If your site do NOT use Lmod, then make sure that XALT's ld is
  always found before the regular ld

XALT does not require that your site use Lmod has your module system.
However, it does require somehow that you make XALT's ld be found in
the path before the real "ld".  Lmod has a special feature that builds
paths in priority order.  This means that Lmod guarantees that XALT's
path appears before other when the other modules are loaded.  If all
your system has is /bin/ld then having XALT's ld found first in the
path is easy.  But modern GCC compilers have their own ld so some
effort will be required to make XALT's ld appear first in $PATH.



