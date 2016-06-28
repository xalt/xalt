# This is the config file for specifying tables necessary to configure XALT:

# The patterns listed here are the hosts that can track executable with XALT.
# Typical usage is that compute nodes track executable with XALT while login
# nodes do not.
#
# Note that linking an executable is everywhere and is independent of
# hostname_patterns

hostname_patterns = [
  '^node[0-9]'
  ]

# Acceptance is done before the ignore list so put in here the
# absolute path of program you want to run that would otherwise
# be in ignore list.  For example /usr/bin/ddt might be a program
# your site would like to track but all other programs in /usr/bin
# are ignored:

accept_path_patterns = [
  '^/usr/bin/ddt'
  ]


# If a path is not in the acceptance list then it is compared
# against the ignore list.  If the path matches any in this list,
# tracking the executable is turned off.

ignore_path_patterns = [
  #Ignore system directories
  r'^/sbin/',
  r'^/bin/',
  r'^/etc',
  r'^/usr',
  r'^/root',

  #Compilers directories
  r'/software/icc/',
  r'/software/ifort/',
  r'/software/GCC/',
  r'/software/GCCcore/',
  r'/software/binutils/',
  r'/software/impi/',
  r'/software/OpenMPI/',
  r'/software/MPICH/',
  r'/software/MPICH2/',
  r'/software/MVAPICH2/',
  r'/software/PGI/',
  
  # Other commands
  r'/cmake$',
  r'/make$',

  # Shells not in /bin
  r'/ash$',
  r'/bash$',
  r'/csh$',
  r'/dash$',
  r'/fish$',
  r'/ksh$',
  r'/pdksh$',
  r'/sh$',
  r'/tcsh$',
  r'/zsh$',

  # Configure and Cmake generated executables.
  r'/conftest$',
  r'/CMakeTmp/cmTryCompileExec[0-9][0-9]*$',
  r'/CMakeTmp/cmTC_[a-f0-9][a-f0-9]*$',

  # XALT Commands
  r'/xalt_file_to_db$',
  r'/xalt_syslog_to_db$',
  r'/xalt_configuration_report$/'
  ]

#------------------------------------------------------------
# Note: The entire environment of key-value pairs are stored in
# the database as a compressed blob.  The entire environment can
# then be reported but it isn't searchable via SQL statements.  

# XALT also filters the environment variables. Those variables
# which pass through the filter are save in an SQL table that is
# controllable via sql commands.
# 
# To be saved a variables name must be found in the accept list
# and not found in the ignore list. For example, the accept list
# might allow for any variables that contain 'PATH' but the
# ignore list causes a variable named DDTPATH or INFOPATH to
# be ignored.

accept_env_patterns = [
  r'^I_MPI.*',
  r'^KMP.*',
  r'^LD_LIBRARY_PATH',
  r'^LOADEDMODULES$',
  r'^MODULEPATH$',
  r'^OMP.*',
  r'^PATH$',
  r'^PYTHON.*',
  r'^R_.*',
  r'^TARG$',
  r'^_LMFILES_$'
  ] 

ignore_env_patterns = [
  r'^I_MPI_DAPL_PROVIDER',
  r'^I_MPI_EXTRA_FILESYSTEM',
  r'^I_MPI_FABRICS',
  r'^I_MPI_OFA_ADAPTER_NAME',
  r'^OMP_NUM_THREADS$',
  r'^__.*'
  ]  


  
