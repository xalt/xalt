Downloading XALT and Configuring it for your site
-------------------------------------------------

The steps to configure XALT are broken down into two major parts.  The
first steps are covered here.  The second part of the configuration
namely the filtering is covered next.

#. Download XALT
#. Tell XALT the name of your cluster.
#. Use your system gcc/g++ and what to do if it is old.
#. Specify how XALT should transmit data to a VM
#. Track MPI and/or Non-MPI?
#. Track GPU usage.
#. Function tracking.

Download XALT
^^^^^^^^^^^^^

From its Github page you can download XALT:

   $ git clone https://github.com/xalt/xalt.git

Setting the Name of your Cluster
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

XALT has the concept of a **syshost** which descriptive name that XALT
uses as the name of your cluster. A site may have one or more
cluster.  Each cluster should have a unique name for each.  XALT is
designed to have a separate database for each cluster.

There are several ways to specify the cluster name:

#. Hardcode the name of the cluster at configure time.
#. Use the nth name from executing: hostname -f
#. Read a file
#. Use a mapping table to convert a regex into a cluster name.
#. Strip the trailing numbers from a hostname
#. Use an environment variable.


Hardcode the name at configure time
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can specify the name of your cluster with the configure step::

    --with-syshostConfig=hardcode:alice

would define the name of your cluster to "alice"

Nth name from hostname
~~~~~~~~~~~~~~~~~~~~~~

At TACC, our computers are named: **node** . **cluster**
.tacc.utexas.edu.  So we use the Nth name method to extract the
cluster name from hostname by doing:

   --with-syshostConfig=nth_name:2

which tells XALT to grap the second name from our hostname.

Read a file
~~~~~~~~~~~

A site could put the name of the cluster in a file (say
/etc/my_hostname.txt) which contains the cluster name.  This is useful
on certain manufacturers control the name of the compute nodes with no
easy way to change it.

    --with-syshostConfig=file:/etc/my_hostname.txt

A Mapping Table in json
~~~~~~~~~~~~~~~~~~~~~~~

A json table can be placed into a file (say my_mapping.json) as follows::

   {
     "regex1" : "cluster1",
     "regex2" : "cluster2",
   }

The configure line would then have::

    --with-syshostConfig=mapping:my_mapping.json

where my_mapping.json would be in the same directory in the configure
script.

Strip the trailing numbers from a hostname
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some sites name their login nodes::

  aliceX

and compute nodes::

  aliceWXYZ

where W,X,Y,Z are digits and the name of the cluster is alice.  Sites
with this naming convention can use::

    --with-syshostConfig=strip_nodename_numbers

Use an environment variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is probably the least reliable method of specifying the name of
your cluster.  Users can delete and some programs tightly control the
environment (like scons).  Still a site can use::

    --with-syshostConfig=env_var:MY_HOSTNAME

XALT use use the value of the environment variable $MY_HOSTNAME to
specify the name of the cluster.


Turning off command line tracking
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sites may wish to not track the user's command line in the database.
This can be accomplished by configuring with::

   --with-cmdlineRecord=no


Built XALT with gcc/g++ 4.8.5 or newer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can build XALT with the system gcc/g++ as long as it is at least
version 4.8.5 or newer.  As of version 2.8.4+, XALT copies all the
required shared libraries to the install directory.

If your system gcc/g++ is older (like on Centos 6), please install and
use a more modern version of gcc/g++.

Defining $PATH used by XALT programs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

XALT must protect itself from changes to $PATH set by the user.  So
instead of hard-coding the PATH to each executable used by XALT, It
uses **--with-systemPath** to set $PATH used by all executables. By
default, XALT uses */usr/bin:/bin* for $PATH to find the executables
it needs.  Your site can override this default with::

   --with-systemPath=/usr/bin:/bin:/usr/local/bin

  
XALT data transmission
^^^^^^^^^^^^^^^^^^^^^^

You need to tell XALT how you want to transmit the generated
data.  There are two choices: either file or syslog.  For testing it
is best to the the "file" transmission style.  By default XALT will
write json records in files in the ~/.xalt.d directory.  XALT will
create that directory if needed.  It is also possible for XALT to
write all json records into a globally writable location by using the
--with-xaltFilePrefix=/path/to/json/files.  If this is set to
/global/xalt then XALT will write json files in a series of
directories under */global/xalt/*. In particular, the program::

  /opt/apps/xalt/xalt/sbin/create_xalt_directories.sh

is used to create a prime number of directories named 000 to 996
(typically) where the json result files are written.


So you should probably start with the following for testing to write
the json record to files in ~/.xalt.d::

   --with-transmission=file
   
To write to a global location (say /global/xalt), you can do::

   --with-transmission=file --with-xaltFilePrefix=/global/xalt

To place each kind of result file in a separate directory do::

   --with-transmission=file_separate_directory --with-xaltFilePrefix=/global/xalt

Note that if your site uses **--with-xaltFilePrefix=...** then you
must create the directories during the installation process.  See 
:ref:`createXALTdirs` for more information.

To write records to syslog do::

   --with-transmission=syslog

XALT 1 supported the *directdb* transmission style.  This is **NOT**
supported in XALT 2.  XALT 1 only tracked MPI programs.  Since they
are fewer in number the demand on a MySQL database server was not a
problem but with XALT 2 tracking both scalar and MPI programs, the
demands on a database server is too high.  Also this would mean that
every tracked scalar program would have to make a database insert
every time it ran.  This would affect the performance of short
programs. 


Track MPI and/or Non-MPI executables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default XALT tracks two types of programs: scalar and
MPI. The scalar programs are non-mpi programs, 

Finally there are MPI programs.  Note that an MPI capable programs run
with only one task is considered to be scalar by XALT.  So if you only
want to track MPI programs you can do::

    --with-trackScalarPrgms=no


Track GPU usage
^^^^^^^^^^^^^^^

Optionally, XALT can track NVIDIA GPU usage.

You can tell XALT to track GPU usage by configuring it with::

   --with-trackGPU=yes


Note
~~~~

You need to turn on persistence mode on your GPU's.  The following
commands will set two GPU's::

   $ sudo nvidia-smi -pm 1 -i 0
   $ sudo nvidia-smi -pm 1 -i 1

You also need to turn on accounting on the GPU's::

   $ sudo nvidia-smi -am 1

These commands need to be run on every reboot.


Next we cover how to control how XALT filters executables.

Function Tracking
^^^^^^^^^^^^^^^^^

XALT can track function usage. That is XALT can detect that a user
code has called a library found in the site's system module tree. For
this to work a site needs to build a reverseMap file. Site can use
Lmod to build the reverseMap file even if the site doesn't use Lmod
for their users (See :ref:`reverseMap-label` for details).  There is a
slight performance penalty for function tracking.  It requires that
programs be linked twice.  By default XALT assumes that a site is
interested in function tracking but this can be disabled by
configuring XALT like this::

    --with-functionTracking=no

XALT also disables function tracking if the reverseMap file is not
found when attempting to link.  For XALT to use the reverseMap file
during linking site will need to configure XALT with the "etc"
directory specified.  This is the directory location is where the
**reverseMapD** directory can be found.

     --with-etcDir=/path/to/reverseMapD_parent

For example TACC uses::

     --with-etcDir=/tmp/moduleData

where the **reverseMapD** directory can be found.

