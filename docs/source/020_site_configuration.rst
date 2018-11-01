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

Download XALT
^^^^^^^^^^^^^

From its Github page you can download XALT:

   $ git clone https://github.com/Fahey-McLay/xalt.git

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


Using your system gcc/g++
^^^^^^^^^^^^^^^^^^^^^^^^^

You must build XALT with the system gcc/g++. If your system g++
is older than 4.8.5 (on Centos 6 for example) then XALT can support
that system but it is slightly more complicated 
Make sure that when you install XALT that the gcc and g++ are the
system ones.  Note that user executables can be built with any other
compilers and will accept an XALT built with system gcc/g++


If your g++ is older than 4.8.5
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If your site's system g++ is older than version 4.8.5 then XALT can
work but you'll need to install a later version of gcc/g++ and make it
available to XALT.  This requires a couple of modifications.

First you'll need to take a the g++.sh file in the distribution and
make it work for your site. You will also have to use the following
options when configuring XALT::

    CXX=$PWD/g++.sh --with-cxxLDLibraryPath=...

where the LD_LIBRARY_PATH you set in g++.sh you'll need to specify on
the command line used to configure XALT.  For example, one system at
TACC required g++.sh to be::

     #!/bin/bash
     export LD_LIBRARY_PATH=/opt/apps/gcc/4.9.1/lib64:/opt/apps/gcc/4.9.1/lib
     /opt/apps/gcc/4.9.1/bin/g++ "$@"

and the configure line looked like::


     ./configure CXX=$PWD/g++.sh --with-cxxLDLibraryPath=/opt/apps/gcc/4.9.1/lib64:/opt/apps/gcc/4.9.1/lib

XALT data transmission
^^^^^^^^^^^^^^^^^^^^^^

You need to tell XALT how you want to transmit the generated
data.  There are two choices: either file or syslog.  For testing it
is best to the the "file" transmission style.  By default XALT will
write json records in files in the ~/.xalt.d directory.  XALT will
create that directory if needed.  It is also possible for XALT to
write all json records into a globally writable location by using the
--with-xaltFilePrefix=/path/to/json/files.  If this is set to
/global/xalt then XALT will write json files in /global/xalt/$USER.

So you should probably start with the following for testing to write
the json record to files in ~/.xalt.d::

   --with-transmission=file
   
To write to a global location (say /global/xalt), you can do::

   --with-transmission=file --with-xaltFilePrefix=/global/xalt

To write records to syslog do::

   --with-transmission=syslog

Track MPI and/or Non-MPI executables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default XALT tracks three types of programs: scalar, SPSR and
MPI. The scalar programs are non-mpi programs, SPSR are special
scalar programs that you can track the libraries that they use
(e.g. python, R and MATLAB).  This is not required so this can be
delayed until you are ready.  Please see XXX in the manual for more
details about SPSR. 

Finally there are MPI programs.  Note that an MPI capable programs run
with only one task is considered to be scalar by XALT.  So if you only
want to track MPI programs you can do::

    --with-trackScalarPrgms=no
    --with-trackSPSR=no


Track GPU usage
^^^^^^^^^^^^^^^

Optionally, XALT can track NVIDIA GPU usage. You will need the DCGM
library from NVIDIA for this to work.  DCGM is free to download from
https://developer.nvidia.com/data-center-gpu-manager-dcgm.

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
