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
Wherever that is you'll need to remember the path to that command

Please do the following steps:

#. Remember the directory that is contain the ld that clang is using::

      $ LD_PATH="..."

   In other words if the path is /usr/bin/ld then set LD_PATH to
   "/usr/bin". Change LD_PATH to suit your site.

#. Typically $LD_PATH/ld is a symlink so::

      $ cd $LD_PATH
      $ ln -s $(readlink ld) ld.x

#. If $LD_PATH/ld is a file rather than a symlink then do::

      $ cd $LD_PATH
      $ cp ld ld.x
     
#. Replace $LD_PATH/ld with the file found in the XALT installed
   directory called: ld.xalt.  In other words if XALT is installed in
   **/opt/apps/xalt/xalt** then do the following::

      $ cd $LD_PATH
      $ XALT_DIR=/opt/apps/xalt/xalt
      $ rm ld;
      $ cp $XALT_DIR/bin/ld.xalt ld

   **NOTE**: Replace **XALT_DIR=/opt/apps/xalt/xalt** with wherever you have
   installed XALT.

#. Test clang linkage with::

   $ XALT_TRACING=link clang -o hello hello.c
     
     




