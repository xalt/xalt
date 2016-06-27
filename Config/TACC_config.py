# This is the config file for specifying tables necessary to configure XALT:

# The patterns listed here are the hosts that can track executable with XALT.
# Typical usage is that compute nodes track executable with XALT while login
# nodes do not.
#
# Note that linking an executable is everywhere and is independent of
# hostname_patterns

hostname_patterns = [
  '^c[0-9][0-9][0-9]-[0-9][0-9][0-9]'
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
  '^/sbin/',
  '^/bin/',
  '^/etc',
  '^/usr',
  '^/root',
  '^/opt/apps/intel/',
  '^/opt/apps/gcc/',
  '^/opt/apps/lua',
  '^/opt/apps/lmod/',
  '^/opt/apps/shell_startup_debug/',
  '/l/pkg/xalt/',
  '^/opt/apps/xalt/',
  '^/opt/apps/git/',
  '^/opt/apps/cmake/',
  '^/opt/apps/autotools/',
  '^/opt/apps/intel[0-9][0-9_]*/mvapich2/',
  '^/opt/apps/intel[0-9][0-9_]*/impi/',
  '^/opt/apps/gcc[0-9][0-9_]*/mvapich2/',
  '/conftest$',
  '/CMakeTmp/cmTryCompileExec[0-9][0-9]*$',
  '/CMakeTmp/cmTC_[a-f0-9][a-f0-9]*$'
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
  '^LD.*',
  '^LD_LIBRARY_PATH$',
  '^LOADEDMODULES$',
  '^MODULEPATH$',
  '^MKL.*',
  '^MV2_.*',
  '^OFFLOAD.*',
  '^OMP.*',
  '^PATH$',
  '^PYTHON.*',
  '^R_.*',
  '^TACC_AFFINITY_ENABLED$',
  '^TARG$',
  '^_LMFILES_$'
  ] 

ignore_env_patterns = [
  '^I_MPI_DAPL_PROVIDER',
  '^I_MPI_EXTRA_FILESYSTEM',
  '^I_MPI_FABRICS',
  '^I_MPI_OFA_ADAPTER_NAME',
  '^MKL_DIR$',
  '^MKL_INCLUDE$',
  '^MKL_LIB$',
  '^MKLROOT$',
  '^MPICH_HOME$',
  '^MV2_COMM_WORLD.*',
  '^MV2_DEFAULT_TIME_OUT$',
  '^MV2_HOMOGENEOUS_CLUSTER$',
  '^MV2_IBA_HCA$',
  '^MV2_NODE_ID$',
  '^MV2_NUM_NODES_IN_JOB$',
  '^MV2_USE_HUGEPAGES$',
  '^MV2_USE_OLD_BCAST$',
  '^MV2_USE_RING_STARTUP$',
  '^OMP_NUM_THREADS$',
  '^__.*'
  ]  


  
