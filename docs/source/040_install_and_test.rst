Running ./configure and building XALT
-------------------------------------

Installing XALT is different from most applications.  You can only
have one active XALT running at a time.   It is common
place to install packages say /apps/xalt/<xalt_version> with
configure.  However you do not want to have the prefix include the
"xalt/xalt_version" as part of prefix.

   **NOTE** DO NOT PUT THE VERSION OF XALT IN THE CONFIGURATION LINE!!

Instead you should specify the directory above xalt for the prefix
value.  So if you would like to install version 2.0 of xalt in
/apps/xalt/2.0 you SHOULD set prefix to /apps.  XALT will install
itself in /apps/xalt/2.0 and make a symlink from /apps/xalt/xalt to
/apps/xalt/2.0.  This way /apps/xalt/xalt will always point to the
latest version of the XALT software.

If you do not follow this advice then you will have trouble with
installing future versions of XALT.

Typical configure and build instructions for stampede2
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here is how we configure XALT for stampede2::

    $ cd xalt
    $ ./configure --prefix=/opt/apps              \
      --with-syshostConfig=nth_name:2             \
      --with-config=Config/TACC_config.py         \
      --with-transmission=syslog                  \
      --with-MySQL=no
    $ make install

If the current version of XALT is X.Y.Z then this will install XALT in
/opt/apps/xalt/X.Y.Z and /opt/apps/xalt/xalt will point to it.

XALT modulefile
^^^^^^^^^^^^^^^

Below is the XALT modulefile you need to specify to use the
package.  The first one is a lua modulefile that can be used with Lmod::


  setenv("XALT_EXECUTABLE_TRACKING",       "yes")

  local base  = "/opt/apps/xalt/xalt"
  local bin   = pathJoin(base,"bin")

  prepend_path{"PATH",          bin, priority="100"}
  prepend_path("LD_PRELOAD",    pathJoin(base,"$LIB/libxalt_init.so"))
  prepend_path("COMPILER_PATH", bin)

  ------------------------------------------------------------
  -- Only set this in production not for testing!!!
  setenv("XALT_SCALAR_AND_SPSR_SAMPLING",  "yes")  

The following is a TCL modulefile::

  setenv XALT_EXECUTABLE_TRACKING       yes

  prepend-path  PATH            /opt/apps/xalt/xalt/bin  100
  prepend-path  LD_PRELOAD      /opt/apps/xalt/xalt/\$LIB/libxalt_init.so
  prepend-path  COMPILER_PATH   /opt/apps/xalt/xalt/bin

  ############################################################
  ## Only set this is production not for testing!!!
  setenv XALT_SCALAR_AND_SPSR_SAMPLING  yes


XALT does not require that your site use Lmod has your module system.
However, it does require somehow that you make XALT's ld be found in
the path before the real "ld".  Lmod has a special feature that builds
paths in priority order.  This means that Lmod guarantees that XALT's
path appears before other when the other modules are loaded.  If all
your system has is /bin/ld then having XALT's ld found first in the
path is easy.  But modern GCC compilers have their own ld so some
effort will be required to make XALT's ld appear first in $PATH.

  **Note**: If you do NOT use Lmod, then make sure that XALT's ld is
  always found before the regular ld



