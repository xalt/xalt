Creating a config.py file for your site
---------------------------------------


XALT requires a configuration file written in python.  There are
examples in the Config directory.  In particular, the file
Config/TACC_config.py is the configuration we use at TACC.  This file
defines three types of filtering.  One is hostname filtering.  It is
typical to only track program execution on compute nodes.::

   **Note** that linking of executable is tracked on both login and compute nodes.

The second type of filtering is by executables path. Finally there is
filtering of the environment variables.  It turns out that storing the
environment is expensive in terms of space in the database.  It is
important to chose a very limited set of variables to save, on the
order of 5 to 10 variables per execution.  For details about this file
please read over the Config/TACC_config.py file for details.  Also pay
attention to the commas.

For testing purposes, you might consider using the
Config/rtm_config.py as your model for your config.py file and then
switch to converting the TACC_config.py file for production.

XALT supports scalar (non-MPI) program sampling based on execution
time.  If you have setup XALT to track scalar or SPSR programs then
you can also control what percentage will be sampled.  Note that the
environment variable:

    XALT_SCALAR_AND_SPSR_SAMPLING=yes

must be set to "yes" in order for sampling to occur.  If this variable
is unset or is not set to "yes" then all scalar program execution are
tracked.  This designed this way to make testing easier.

XALT also provides tracking of certain scalar programs (assuming your
config.py file turns this option on) that can track the internal
package use.  These programs could be python, R and matlab.  For these
programs a start record must be produced.  By default XALT will track
100% of these programs, but this might be overwhelming so XALT allows
a site to specify a percentage of these jobs to be tracked.  See
Config/TACC_config.py for more details.

Also note that sites can control the sampling rate for SPSR programs
by setting the env var. XALT_SPSR_SAMPLING_RATE to a number between
0.0 and 1.0 where 0.0 would mean no tracking at all of SPSR programs
and 1.0 would mean that 100% of SPSR programs are tracked.
