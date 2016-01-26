Quick Start Guide - Prerequistes and Decisions
==============================================

Prerequisites
^^^^^^^^^^^^^
- Python 2.6 or later

- Python MySQLDb module

- Servers: MySQL with proper IP ranges opened for client machines

Install XALT
^^^^^^^^^^^^
XALT may be downloaded from either sourceforge or github:

  http://sourceforge.net/projects/xalt/

  http://github.com/Fahey-McLay/xalt/

Decisions: Installation Location, Database(s), Intercepting
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
**Installation Location and the Database**

Choose an installation directory, the directory will be referred to as XALT_DIR

**Database Decisions**

For XALT on multiple machines, you need to decide if you want to have one database or multiple databases.

Either mode is supported.

You will need a file/syslog to datbase scripts for each machine for both options.

**Lmod/ReverseMap**

Lmod on multiple machines
- Machines have different software installations (module lists) AND you want ReverseMAP support.
- Machines with different architectures.
Lmod provides the spider utility which creates the map. TLC module system does not need to be replaced.

**Intercepting: linkers and launchers**
If you have multiple code launchers and linkers, you must choose a method to intercept them. There are currently three options for wrapping.
	#. Aliasing  
	#. XALT modulefile
	#. Move the launcher
	
	:doc:`070_interception`

**File Location**

Choose a location for files the following files: xalt_db.conf and the reverse MapD directory. We recommend they go in XALT_DIR/etc, but
In the case that you want the xalt_db.conf file hidden or secured, by overrunning at runtime with XALT_ETC_DIR or with the configuration option*::

	--with-etcDir=ans 

Note: you will need to have an XALT_ETC_DIR directory for each machine.


Next Steps - Installation
^^^^^^^^^^^^^^^^^^^^^^^^^
  :doc:`010_installation`
