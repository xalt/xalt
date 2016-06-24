# This is the config file for specifying tables necessary to configure XALT:

# The patterns listed here are the hosts that can run XALT.  Typical usage is that
# compute nodes run XALT while login nodes do not.

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
  '^/sbin/',
  '^/bin/',
  '^/etc',
  '^/usr',
  '^/root',

  '/cmake$',

  # Gnu Compilers
  '/gcc$',
  '/g++$',
  '/gfortran$',
  '/git$',

  #Intel compilers
  '/icc$',
  '/icpc$',
  '/ifort$',

  '/make$',

  # Standard MPI Compiler Wrappers
  '/mpiCC$',
  '/mpicc$',
  '/mpicxx$',
  '/mpif77$',
  '/mpif90$',
  '/mpifort$',
  '/mpifc$',
  '/mpigcc$',
  '/mpigxx$',
  '/mpiicc$',
  '/mpiicpc$',
  '/mpiifort$',
  '/mpiexec.hydra$',

  # OpenMPI Compiler Wrapper
  '/ompi_info$',
  '/opal_wrapper$',
  '/orterun$',

  # Portland Group Compilers
  '/ftn$',
  '/pgcc$',
  '/pgCC$',
  '/pgc\+\+$',
  '/pgf77$',
  '/pgfortran$',
  '/pgf90$',

  # IBM XL compilers
  '/xlc$',
  '/xlC$',
  '/xlf$',
  '/xlf90$',

  '/vtwrapper$',

  # Configure and Cmake generated executables.
  '/conftest$',
  '/CMakeTmp/cmTryCompileExec[0-9][0-9]*$',
  '/CMakeTmp/cmTC_[a-f0-9][a-f0-9]*$',

  # XALT Commands
  '/xalt_file_to_db$',
  '/xalt_syslog_to_db$',
  '/xalt_configuration_report$/'
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
  '^I_MPI.*',
  '^KMP.*',
  '^LD_LIBRARY_PATH',
  '^LOADEDMODULES$',
  '^MODULEPATH$',
  '^OMP.*',
  '^PATH$',
  '^PYTHON.*',
  '^R_.*',
  '^TARG$',
  '^_LMFILES_$'
  ] 

ignore_env_patterns = [
  '^I_MPI_DAPL_PROVIDER',
  '^I_MPI_EXTRA_FILESYSTEM',
  '^I_MPI_FABRICS',
  '^I_MPI_OFA_ADAPTER_NAME',
  '^OMP_NUM_THREADS$',
  '^__.*'
  ]  


  
