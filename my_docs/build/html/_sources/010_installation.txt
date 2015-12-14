Installation
=================

Though not required, installation an operation of XALT is easier with pre-requisite software: lua and lmod.


Install XALT
^^^^^^^^^^^^
XALT may be downloaded from either sourceforge or github:

  http://sourceforge.net/projects/xalt/

  http://github.com/Fahey-McLay/xalt/

.. _Lmod:

Lmod Installation
^^^^^^^^^^^^^^^^^^

Get Lmod-5.8rc2 or greater from from sourceforge::

	http://sourceforge.net/projects/lmod/files/



Or get Lmod-5.8rc2 or greater from::

  git clone /Lmod.git

::

	download Lmod-<ver>.tar.gz from github.zod



Installation instructions and documentation for Lmod:
  http://lmod.readthedocs.org/

Installation Steps
^^^^^^^^^^^^^^^^^^

First, untar the XALT file, change the directory to XALT, configure, then install::

	$ > cd xalt	
	$ > ./configure --prefix=$XALT_DIR --with-etcDir=$XALT_ETC_DIR
	$ > make
	$ > make install

Next, remove job launchers that are not supported on your system. Launchers currently on XALT include: aprun, ibrun, mpirun, mpiexec, srun.

For removing launchers on a Cray, which only supports 'aprun'::

	$> cd $XALT_DIR/bin
	$> rm ibrun* mpirun mpiexec srun
	

Next, build a file for ReverseMap and libmap. 

ReverseMap associates libraries and object files with the modulefile that specifies them. 

Libmap lists the library files (.a and .so) whose call to their function will be tracked by XALT. 

ReverseMap and Libmap are stored in the same file.

On a cluster::


	$ > $LMOD_DIR/lmod/lmod/libexec/spider -o jsonReverseMapT $MODULEPATH
	$ > {XALT_ETC_DIR}/reverseMapD/jsonReverseMapT.json

It is important to use the same filename and directory "reverseMapD" as they
are used by conventions in XALT.

On a Cray (XC, XE, XK), the process is a bit more involved, so a script has been
provided under contrib/ directory in the XALT source distribution to make it
easier. Use this script and run::

	$> xalt/contrib/build_reverseMapT_cray/cray_build_rmapT.sh $XALT_ETC_DIR

Once the ReverseMap file is built, add libmap to is using as::

	$> $XALT_DIR/sbin/xalt_rmap_lmap.py 

Finally, create the database and tables to hold the data. 

Create the file to hold database credentials::

	$> cd $XALT_ETC_DIR
	$> python $XALT_DIR/sbin/conf_create.py #-- create xalt_db.conf
In this step, make sure that the credential you created have the necessary privileges to create tables, etc. If you want the database credential to be more restrictive,recreate the file to hold database credentials with the desired privilege account. 


Create the database and tables for XALT (need to be run from $XALT_ETC_DIR)::

	$> cd $XALT_ETC_DIR
	$> python $XALT_DIR/sbin/createDB.py
	

Next Steps - Testing
^^^^^^^^^^^^^^^^^^^^
  :doc:`040_testing`
