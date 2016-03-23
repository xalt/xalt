# This is the config file for specifying tables necessary to configure XALT:

hostname_patterns = [
  '.*'                    # match  all hosts
  ]

accept_path_patterns = [
  '^/usr/bin/ddt'
  ]

ignore_path_patterns = [
  '^/sbin/',
  '^/bin/',
  '^/etc',
  '^/usr',
  '^/opt/apps/intel/',
  '^/opt/apps/gcc/',
  '^/opt/apps/lua',
  '^/opt/apps/lmod/',
  '^/opt/apps/icon/',
  '^/opt/apps/noweb/',
  '^/opt/apps/shell_startup_debug/',
  '^/opt/apps/xalt/',
  '^/opt/apps/intel-[0-9][0-9_]*/mpich/',
  '^/opt/apps/intel-[0-9][0-9_]*/openmpi/',
  '^/opt/VBoxGuestAdditions'
  ]

accept_env_patterns = [
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
  '^R_*',
  '^SHELL$',
  '^TARG$',
  '^_LMFILES_$'
  ] 

ignore_env_patterns = [
  '^DDTPATH$',
  '^INFOPATH$',
  '^LMOD*',
  '^MAKE_INC_PATH$',
  '^MANPATH$',
  '^MIC_ENV_PREFIX$',
  '^MIC_TACC.*_DIR$',
  '^MIC_TACC.*_INC',
  '^MIC_TACC.*_LIB$',
  '^MKLROOT$',
  '^MODULEPATH_ROOT$',
  '^MPICH_HOME$',
  '^PKG_CONFIG_PATH$',
  '^__.*'
  ]  
  
