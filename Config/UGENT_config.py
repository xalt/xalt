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

  # Gnu Compilers
  r'/gcc$',
  r'/g\\+\\+$',
  r'/gfortran$',

  # Random commands
  r'/cmake$',
  r'/git$',
  r'/ld$',

  #Intel compilers
  r'/icc$',
  r'/icpc$',
  r'/ifort$',

  r'/make$',

  # Standard MPI Compiler Wrappers
  r'/mpiCC$',
  r'/mpicc$',
  r'/mpicxx$',
  r'/mpif77$',
  r'/mpif90$',
  r'/mpifort$',
  r'/mpifc$',
  r'/mpigcc$',
  r'/mpigxx$',
  r'/mpiicc$',
  r'/mpiicpc$',
  r'/mpiifort$',
  r'/mpiexec.hydra$',

  # OpenMPI Compiler Wrapper
  r'/ompi_info$',
  r'/opal_wrapper$',
  r'/orterun$',

  # Portland Group Compilers
  r'/ftn$',
  r'/pgcc$',
  r'/pgCC$',
  r'/pgc\\+\\+$',
  r'/pgf77$',
  r'/pgfortran$',
  r'/pgf90$',

  # IBM XL compilers
  r'/xlc$',
  r'/xlC$',
  r'/xlf$',
  r'/xlf90$',

  r'/vtwrapper$',

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


  
