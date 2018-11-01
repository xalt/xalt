Load Json records via the syslog transmission style
---------------------------------------------------


If you are syslog, you will follow these instructions instead of the
ones found :ref:`json_by_file-label`

This discussion assume that you are using **rsyslog** as your syslog
daemon.  Also this discussion assume that you have a "master" node
where all syslog are collected from your login and compute nodes.


Modify /etc/rsyslog.conf
^^^^^^^^^^^^^^^^^^^^^^^^

You will need to modify /etc/rsyslog.conf on your login and compute
and master nodes. So on every login and compute node you modify to
/etc/rsyslog.conf to have the following lines::


    # Send XALT data to "master"
    if $programname contains 'XALT_LOGGING' then @@XXX.YYY.ZZZ.WWW
    &~

where XXX.YYY.ZZZ.WWW is the IP address of "master".  Then on
"master"'s /etc/rsyslog.conf you ship all the XALT logging messages
on::

    # Send XALT data to XALT VM.
    if $programname contains 'XALT_LOGGING' then @AAA.BBB.CCC.DDD
    &~

Where you have put in the ip or name of your VM instead of
AAA.BBB.CCC.DDD.  Note too, that whether you use @@ or @ depends on
whether you are using the TCP (@@) or UDP (@) transport protocol. 

Setup /etc/rsyslog.conf and log rotation on your VM.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In our /etc/rsyslog.conf file there is a line::

     $IncludeConfig /etc/rsyslog.d/*.conf

So we created a file named /etc/rsyslog.d/xalt.conf  has the
following::

    $Ruleset remote
    if $programname contains 'XALT_LOGGING_maverick' then /data/log/xalt_maverick.log
    &~
    if $programname contains 'XALT_LOGGING_stampede2' then /data/log/xalt_stampede2.log
    &~
    $Ruleset RSYSLOG_DefaultRuleset

    # provides UDP syslog reception
    $ModLoad imudp
    $InputUDPServerBindRuleset remote
    $UDPServerRun 514

Please restart rsyslog so that the new setting will be noticed.

So XALT tags each syslog message with XALT_LOGGING_<syshost> where
syshost is the name that you have configured XALT with. On our VM, the
/data partition is large so we write the log files there.  You WILL have
to modify the above to match your site.

You will want to set up log rotation on the these syslog files.  Here
are ours in /etc/logrotate.d/xalt::

   /data/log/xalt_maverick.log{
      missingok
      copytruncate
      rotate 20
      daily
      create 644 root root
      notifempty
   }
   /data/log/xalt_stampede2.log{
      missingok
      copytruncate
      rotate 20
      daily
      create 644 root root
      notifempty
   }  


Create directory structure for loading json records
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use the installed XALT on the VM to load the syslog files.  Then
use the shell script below to read the syslog files in the database.
Lets assume that you have installed XALT in the account named swtools,
in ~/xalt/xalt on the VM and that you have in the swtools account
create the following structure::

   $ tree ~/process_xalt/
   process_xalt/
   ├── maverick
   │   ├── createDB.py -> ../../xalt/xalt/sbin/createDB.py
   │   ├── reverseMapD
   │   │   ├── jsonReverseMapT.json
   │   └── xalt_maverick_db.conf
   ├── stampede2
       ├── createDB.py -> ../../xalt/xalt/sbin/createDB.py
       ├── reverseMapD
       │   ├── jsonReverseMapT.json
       └── xalt_stampede2_db.conf



The point of this structure is to have a location for each cluster
your site is saving and a location where the reverseMap file,
jsonReverseMapT.json is located.

You will have to have a cron job which builds the reverseMap daily on
each cluster.  Then that file will have to be copied or somehow made
available on the VM.



Create script to process syslog data from XALT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Add this script in the ~swtools which loads the syslog files
after you have modified this script to match your site by setting
GCC_PATH and GCC_LIB_PATH as necessary.  Lets assume that this script
is called "store_xalt_data"::

    #!/bin/bash
    # -*- shell-script -*-

    # Set GCC_PATH to a g++ with supports C++ 11
    # Set GCC_LIB_PATH to the shared libraries that the g++ needs to support C++ 11
    # Set PY_PATH to a python that has support for python-MySQL package.

    SWTOOLS=~swtools
    export PATH=$SWTOOLS/xalt/xalt/bin:$SWTOOLS/xalt/xalt/sbin:$GCC_PATH:$PY_PATH:$PATH
    export LD_LIBRARY_PATH=$GCC_LIB_PATH

    store_xalt_data ()
    {
        cluster=$1
        shift 1;
        clusterDir=$SWTOOLS/process_xalt/$cluster
        for log in "$@"; do
            echo xalt_syslog_to_db.py --syslog $log --syshost $cluster --reverseMapD $clusterDir/reverseMapD --leftover $clusterDir/leftover.log --confFn $clusterDir/xalt_${cluster}_db.conf
                 xalt_syslog_to_db.py --syslog $log --syshost $cluster --reverseMapD $clusterDir/reverseMapD --leftover $clusterDir/leftover.log --confFn $clusterDir/xalt_${cluster}_db.conf
            echo "date: $(date)"
        done
    }

    echo "Start Date: $(date)"

    store_xalt_data "$@"



Automate processing of syslog records from XALT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As root run the script that you created in step 6.3.  Assuming
that your clusters are name maverick and stampede2 you would run them
as follows and assuming that the syslog files are in /data/log::

    $ ~swtools/process_xalt/store_xalt_data maverick  /data/log/xalt_maverick.log-*;
    $ ~swtools/process_xalt/store_xalt_data stampede2 /data/log/xalt_stampede2.log-*;
 
Once you have the process setup you can delete the syslog files.
