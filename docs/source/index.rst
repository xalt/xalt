XALT 2: Tracking user executables and library usage on your cluster
===================================================================

PURPOSE
^^^^^^^
XALT 2 is a tool to allow a site to track user executables and library
usage on a cluster.  When installed it can tell a site what are the
top executables by Node-Hours or by the number of users or the number
of times it is run.  XALT 2 also tracks library usage as well.  XALT 2
can also track package use by R, MATLAB or Python.  It tracks both MPI
and non-MPI programs.  

OVERVIEW
^^^^^^^^

XALT  does its tracking by ''hijacking'' the linker and by use
of the environment variable **LD_PRELOAD** to have code run before and
after main().  And yes it works with Fortran programs as well.  Since
XALT 2 could track every single program on your cluster, XALT 2 allows
for site controllable filtering.  Otherwise an data collection scheme
would get overwhelmed.


Installing XALT2
^^^^^^^^^^^^^^^^

Installing XALT 2 is both straightforward and complicated.  It is
straightforward in that it only requires a configure step and **make
install**.  But it is complicated because a site will be required to
taylor the filtering to meet their site's needs.  The install steps
are divided into the following steps.

.. toctree::
   :maxdepth: 1

   010_prereqs
   020_site_configuration
   030_site_filtering
   040_install_and_test
   050_reverse_map


              
   


