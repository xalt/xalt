# This is the config file for specifying tables necessary to configure XALT:

# The patterns listed here are the hosts that can run XALT.  Typical usage is that
# compute nodes run XALT while login nodes do not.

hostname_patterns = [
  'c[0-9][0-9][0-9]-[0-9][0-9][0-9]'
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
  '^/opt/apps/intel[0-9][0-9_]*/mvapich2/',
  '^/opt/apps/intel[0-9][0-9_]*/impi/',
  '^/opt/apps/gcc[0-9][0-9_]*/mvapich2/',
  ]

#------------------------------------------------------------
# Note: The entire environment of key-value pairs are stored in
# the database as a compressed blob.  The entire environment can
# then be reported but it isn't searchable via SQL statements.  

# XALT also filters the environment variables. Those variables
# which pass through the filter are save in an SQL table that is
# controllable via sql commands.
# 
# Each environment variable must go through the Accept list
# followed by the reject list.  To be saved a variables name must be 
# found in the accept list and not found in the ignore list.
# For example, the accept list might allow for any variables that contain
# 'PATH' but the ignore list causes a variable named DDTPATH to be ignored.

accept_env_patterns = [
  '^TACC_AFFINITY_ENABLED$',
  '^.*PATH.*',
  '^I_MPI.*',
  '^KMP.*',
  '^LC_.*',
  '^LD.*',
  '^LOADEDMODULES$',
  '^MIC_.*',
  '^MKL.*',
  '^MPICH_.*',
  '^MV2_.*',
  '^OFFLOAD.*',
  '^OMP.*',
  '^PYTHON.*',
  '^R_.*',
  '^SHELL$',
  '^TARG$',
  '^_LMFILES_$'
  ] 

ignore_env_patterns = [
  '^DDTPATH$',
  '^INFOPATH$',
  '^LMOD*',
  '^LUA*',
  '^LC_COLLATE$',
  '^MAKE_INC_PATH$',
  '^MANPATH$',
  '^MIC_ENV_PREFIX$',
  '^MIC_TACC.*_DIR$',
  '^MIC_TACC.*_INC',
  '^MIC_TACC.*_LIB$',
  '^MKLROOT$',
  '^MKPATH$',
  '^MODULEPATH_ROOT$',
  '^MPICH_HOME$',
  '^PKG_CONFIG_PATH$',
  '^__.*'
  ]  
