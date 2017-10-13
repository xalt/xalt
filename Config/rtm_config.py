# This is the config file for specifying tables necessary to configure XALT:

# Here are patterns for non-mpi programs to produce a start-record.
# Normally non-mpi programs (a.k.a.) scalar executables only produce
# an end-record, but programs like R and python that can have optional data
# such as R and python must have a start-record.

scalar_prgm_start_record = [
    r'/python[0-9][^/][^/]*$',
    r'/R$'
    ]

# The patterns listed here are the hosts that can track executable with XALT.
# Typical usage is that compute nodes track executable with XALT while login
# nodes do not.
#
# Note that linking an executable is everywhere and is independent of
# hostname_patterns

hostname_patterns = [
  '.*'                    # match  all hosts
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
  r'^/sbin/',
  r'^/bin/',
  r'^/etc',
  r'^/usr',
  r'^/root',
  r'/gcc$',
  r'/g\\+\\+$',
  r'/gfortran$',
  r'/git$',
  r'/icc$',
  r'/icpc$',
  r'/ifort$',
  r'/lua$',
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
  r'/hydra_pmi_proxy$',
  r'/ompi_info$',
  r'/opal_wrapper$',
  r'/orterun$',
  r'/vtwrapper$',
  r'/conftest$',
  r'/CMakeTmp/cmTryCompileExec[0-9][0-9]*$',
  r'/CMakeTmp/cmTC_[a-f0-9][a-f0-9]*$',
  r'/xalt_file_to_db$',
  r'/xalt_syslog_to_db$',
  r'/xalt_configuration_report$'
  ]

path_patterns = [
    ['KEEP',  r'^\/usr\/bin\/ddt'],
    ['SKIP',  r'^\/sbin\/.*'],
    ['SKIP',  r'^\/bin\/.*'],
    ['SKIP',  r'^\/etc\/.*'],
    ['SKIP',  r'^\/usr\/.*'],
    ['SKIP',  r'^\/root\/.*'],
    ['SKIP',  r'.*\/gcc'],
    ['SKIP',  r'.*\/g\+\+'],
    ['SKIP',  r'.*\/gfortran'],
    ['SKIP',  r'.*\/git'],
    ['SKIP',  r'.*\/icc'],
    ['SKIP',  r'.*\/icpc'],
    ['SKIP',  r'.*\/ifort'],
    ['SKIP',  r'.*\/lua'],
    ['SKIP',  r'.*\/mpiCC'],
    ['SKIP',  r'.*\/mpicc'],
    ['SKIP',  r'.*\/mpicxx'],
    ['SKIP',  r'.*\/mpif77'],
    ['SKIP',  r'.*\/mpif90'],
    ['SKIP',  r'.*\/mpifort'],
    ['SKIP',  r'.*\/mpifc'],
    ['SKIP',  r'.*\/mpigcc'],
    ['SKIP',  r'.*\/mpigxx'],
    ['SKIP',  r'.*\/mpiicc'],
    ['SKIP',  r'.*\/mpiicpc'],
    ['SKIP',  r'.*\/mpiifort'],
    ['SKIP',  r'.*\/mpiexec.hydra'],
    ['SKIP',  r'.*\/hydra_pmi_proxy'],
    ['SKIP',  r'.*\/ompi_info'],
    ['SKIP',  r'.*\/opal_wrapper'],
    ['SKIP',  r'.*\/orterun'],
    ['SKIP',  r'.*\/vtwrapper'],
    ['SKIP',  r'.*\/conftest'],
    ['SKIP',  r'.*\/CMakeTmp\/cmTryCompileExec[0-9][0-9]*'],
    ['SKIP',  r'.*\/CMakeTmp\/cmTC_[a-f0-9][a-f0-9]*'],
    ['SKIP',  r'.*\/xalt_file_to_db'],
    ['SKIP',  r'.*\/xalt_syslog_to_db'],
    ['SKIP',  r'.*\/xalt_configuration_report']
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

env_patterns = [
    [ 'SKIP', r'^MKLROOT=.*' ],
    [ 'SKIP', r'^MKL_DIR=.*' ],
    [ 'SKIP', r'^MKL_INCLUDE=.*' ],
    [ 'SKIP', r'^MKL_LIB=.*' ],
    [ 'SKIP', r'^MPICH_HOME=.*' ],
    [ 'SKIP', r'^MV2_COMM_WORLD=.*'],
    [ 'SKIP', r'^MV2_CPU_BINDING_POLICY=.*' ],
    [ 'SKIP', r'^MV2_DEFAULT_TIME_OUT=.*' ],
    [ 'SKIP', r'^MV2_HOMOGENEOUS_CLUSTER=.*' ],
    [ 'SKIP', r'^MV2_HYBRID_BINDING_POLICY=.*' ],
    [ 'SKIP', r'^MV2_IBA_HCA=.*' ],
    [ 'SKIP', r'^MV2_NODE_ID=.*' ],
    [ 'SKIP', r'^MV2_NUM_NODES_IN_JOB=.*' ],
    [ 'SKIP', r'^MV2_THREADS_PER_PROCESS=.*' ],
    [ 'SKIP', r'^MV2_USE_HUGEPAGES=.*' ],
    [ 'SKIP', r'^MV2_USE_OLD_BCAST=.*' ],
    [ 'SKIP', r'^MV2_USE_RING_STARTUP=.*' ],
    [ 'SKIP', r'^MV2_USE_UD_HYBRID=.*' ],
    [ 'SKIP', r'^OMP_NUM_THREADS=.*' ],
    [ 'SKIP', r'^__.*'],
    [ 'KEEP', r'^I_MPI_INFO_NUMA_NODE_MAP=.*' ],
    [ 'KEEP', r'^I_MPI_INFO_NUMA_NODE_NUM=.*'],
    [ 'KEEP', r'^I_MPI_PIN_INFO=.*'],
    [ 'KEEP', r'^I_MPI_PIN_MAPPING=.*'],
    [ 'KEEP', r'^I_MPI_THREAD_LEVEL=.*'],
    [ 'KEEP', r'^I_MPI_TMI_PROVIDER=.*'],
    [ 'KEEP', r'^LAUNCHER_JID=.*'],
    [ 'KEEP', r'^LD=.*'],
    [ 'KEEP', r'^LD_LIBRARY_PATH=.*'],
    [ 'KEEP', r'^LOADEDMODULES=.*'],
    [ 'KEEP', r'^MODULEPATH=.*'],
    [ 'KEEP', r'^MKL.*'],
    [ 'KEEP', r'^MV2_.*'],
    [ 'KEEP', r'^OFFLOAD.*'],
    [ 'KEEP', r'^OMP.*'],
    [ 'KEEP', r'^PATH=.*'],
    [ 'KEEP', r'^PYTHON.*'],
    [ 'KEEP', r'^R_.*'],
    [ 'KEEP', r'^TACC_AFFINITY_ENABLED=.*'],
    [ 'KEEP', r'^_LMFILES_=.*'],
  ]
