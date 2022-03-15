Integrating XALT with clang compilers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The clang compiler/linker hard codes the path to the linker rather
than performing a path search to find the **ld** program. This means to
integrate XALT with clang compiler will require placing an XALT shell
script where ld exists and moving the real **ld** to **ld.x**

First step is to find what linker clang is calling.  An easy way to do
that is to compile and link a simple c program:

    $ clang -v -o hello hello.c

If you look at the output you'll see the link line that clang uses.
It should be /usr/bin/ld.  


   **NOTE:** If it is not then note that XALT's ld.in
   will need to be modified because it expect that the real linker
   will be called /usr/bin/ld.x after completing the steps below.
   Please report this to the xalt mailing list if clang is using
   something else.

Assuing that clang is using /usr/bin/ld to link.  Then do the following steps:

#. Typically /usr/bin/ld is a symlink so::

      $ cd /usr/bin/
      $ ln -s $(readlink ld) ld.x

#. If /usr/bin/ld is a file rather than a symlink then do::

      $ cd /usr/bin/
      $ cp ld ld.x
     
#. Replace /usr/bin/ld with the file found in the XALT installed
   directory called: ld.xalt.  In other words if XALT is installed in
   **/opt/apps/xalt/xalt** then do the following::

      $ cd /usr/bin
      $ rm ld;
      $ cp /opt/apps/xalt/xalt/bin/ld.xalt ld

   **NOTE**: Replace **/opt/apps/xalt/xalt** with wherever you have
   installed XALT.

#. Test clang linkage with::

   $ XALT_TRACING=link clang -o hello hello.c
     
     




