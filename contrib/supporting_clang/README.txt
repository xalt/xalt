This note describes how to use XALT with the clang compiler/linker.

The problem with the clang compiler/linker is that it hard codes the
path to the linker rather than trying to find ld program through the
PATH variable.

First step is to find what linker clang is calling.  An easy way to do
that is to compile and link a simple c program:

    $ clang -v -o hello hello.c

If you look at the output you'll see the link line that clang uses.
It should be /usr/bin/ld.  If it is not then note that XALT's ld.in
will need to be modified because it looks for /usr/bin/ld.x


Then you'll take the ld.sh in this directory and copy it to /usr/bin/
or where ever the last step told you it was.  The edit the
/usr/bin/ld.sh file to match the correct paths for ld.x and XALT's ld.


On my system ld is a sym-link to ld.bfd.  So I do the following:

    $ cd /usr/bin; rm ld; ln -s ld.bfd ld.x; ln -s ld.sh ld

Then add

   setenv("XALT_ACTIVE","1")

to the xalt modulefile.

