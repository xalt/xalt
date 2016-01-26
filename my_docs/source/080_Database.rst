Database Set Up
----------------

New for Version x.xx
^^^^^^^^^^^^^^^^^^^^

For version x.xx of XALT, there are a few changes.
1. The schema has changed to increase speed, change storage, and track more (MPI/non-MPI)
2. The join tables have been partitioned and are divided by month.

For users with an older version of XALT, we recommend migrating to the new database structure.

Setting Up the Database
^^^^^^^^^^^^^^^^^^^^^^^

To set up the database, run sbin/conf_create.py to create your own xalt_db.conf file to point to your mySQL server.
This file needs to be put in the XALT_ETC_DIR location specified at configuration (--with-etcDir=ans) or by the environment variable XALT_ETC_DIR.

To create the initial database schema, use::

	sbin/createDB.py

This may need to be run from the xalt_db.conf location.

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
