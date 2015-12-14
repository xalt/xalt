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
	



Next Steps - Testing
^^^^^^^^^^^^^^^^^^^^
  :doc:`040_testing`
