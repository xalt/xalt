.. _json_by_file-label:

Load Json records via file transmission style.
----------------------------------------------

You will either load the json records via file or syslog.  Use this
description if you are saving the XALT records using file
transmission style.  Skip these steps if you are using the syslog
approach. 

On each cluster create an account where you have installed XALT with
MySQL support and python-MySQL support with $PREFIX=$HOME.  This means
that ~/xalt/xalt/* will have the XALT package installed.  Lets assume
that this account is named swtools

Create a directory for scripts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create a directory called ~/process_xalt In that directory
create a file to hold the database credentials with conf_create.py::

   $ cd ~/process_xalt; python ~/xalt/xalt/sbin/conf_create.py

This will generate a file named xalt_<dbname>_db.conf.

Create a directory for the reverse map
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create a directory called ~/process_xalt/reverseMapD.  Then build the
reverseMap file in that directory as described in
:ref:`reverseMap-label`.  This reverseMap file has to be regenerated
every time you add modules. So to be safe it is probably best to
rebuild the map every day.

Create Script to load json records into the database
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With these step completed you can now load the generated records
into your database.  You will want a cron job that runs frequently, I
would recommend a cron collection probably at least once an hour.  You
will need some locking mechanism to prevent two or more collections
run at the same time::


   #!/bin/bash

   # get lock, quit if lock is unavailable
   # set trap to clear lock if this script aborts.

   ~swtools/xalt/xalt/sbin/xalt_file_to_db.py --delete      \
     --confFn  ~swtools/process_xalt/xalt_<dbname>_db.conf  \
     --reverseMapD ~swtools/process_xalt/reverseMapD 
   
   # remove lock

This script should be run as root or an account which can transverse
the directories that contain the json record files.

