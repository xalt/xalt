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

Lets assume that XALT_DIR is the directory above where you where you
wish to install XALT (say /apps).

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

