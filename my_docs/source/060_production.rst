Production
==========

reverseMap
^^^^^^^^^^^^^^^^^^
Lmod asissted mapping of libraries back to modulefiles.

When modulefiles are set up with a one-to-one mapping of modules to package installations, Lmods can (usually) create reverse maps without issues.
But, when moduelfiles point to a variety of installations and set environment variables, a reverse map will have many-to-one relationships (i.e. Cray's provided modulefile system). On systems like Cray, for the spider work, it must be run multiple times resulting in muliple reverse maps, which then must be combined to crease a master reverse map.

Reverse maps need to be created/updated everytime a new modulefile or package is installed. 
Integrations for reverse map creation include
1. Part of the software installation process
2. Run a a cron job on a set interval, using a script provided in the XALT software package to update the module name entries in the database.

As mentioned in installation, if you have mulitple machines and one XALT installation, you will need to have a reverse map for each machine. 

Simple command to create reverse map::

	spider -o jsonReverseMapT
	$LMOD_MODULEPATH > rmapD/jsonReverseMapT.json

Cray script, found in the contrib/build_reverseMapT_cray/ (this example, XC30 using Lmod spider utility)::

	darter_build_rmapT.sh 

Wrappers
^^^^^^^^

Some tools have issues with wrappers (i.e. Totalview), and it is easy to unload xalt automatically when loading wrappers with issues. The alternative, automatically loading xalt when wrappers are not being used is much harder.
As a result, a small percentage of links and runs are missed.


syslog
^^^^^^

Syslog is the preferred method for production.  All records first go to the syslog and then are parsed later to go into the database.

Set rsyslog settings and set up a configuration file for syslog and place it in /etc/rsyslog.d/ that we have called xalt_syslog.conf::

	example of how to set this up


Testing Parse syslog file
^^^^^^^^^^^^^^^^^^^^^^^^^

Use xalt_syslog_to_db.py to collect data from syslog::

	python xalt_syslog_to_database.py -reverseMapD=$BASE/etc/reverseMApD /var/log/xalt.log.1

This may be rerun without producing duplicate entries.

To set up rotation on the /var/log/xalt.log file with a log rotate configuration::

	HOW TO HERE

The command above sets a 4 day rotation, minimum recommended rotation is a 2 day rotation. The above command creates a log file that can be read by all non-root users.  Decisions about access to log files must be made on a site-by-site basis.



Next Steps - Data Mining
^^^^^^^^^^^^^^^^^^^^^^^^
  :doc:`070_datamining`
