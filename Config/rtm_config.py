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

#------------------------------------------------------------
# This "table" is use to filter executables by their path
# The value on the left is either KEEP or SKIP.  If the value
# is KEEP then if the path matches the regular expression then
# the executable is acceptable as far as the path test goes.
# If the value on the left is SKIP then if the path matches
# the regular expression then executable is not acceptable so
# no XALT tracking is done for that path.

# This "table" is used to generate a flex routine that processes
# the paths. So the regular express must follow flex rules.
# In particular, in order to match the pattern must match the whole path
# No partial matches allowed.  Also do not use $ to match the
# end of the string.  Finally slash is a special character and must
# be quoted with a backslash.

# The path are conceptionally matched from the first regular 
# expression to the last.  Once a match is found no other later
# matches are checked. The upshot of this is that if you want to
# track say /usr/bin/ddt, but ignore everything in /usr, then keep
# /usr/bin/ddt first and skip /usr/.* after.

# If a path does not match any patterns it is marked as KEEP.

# There are special scalar programs that must generate a start record.
# These are marked as SPSR

path_patterns = [
    ['SPSR'   r'.*\/python[0-9][^/][^/]*'],
    ['SPSR'   r'.*\/R'],
    ['KEEP',  r'^\/usr\/bin\/ddt'],
    ['SKIP',  r'^\/usr\/.*'],
    ['SKIP',  r'^\/sbin\/.*'],
    ['SKIP',  r'^\/bin\/.*'],
    ['SKIP',  r'^\/etc\/.*'],
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
# XALT filter environment variables.  Those variables
# which pass through the filter are save in an SQL table that is
# searchable via sql commands.  The environment variables are passed
# to this filter routine as:
#
#      env_key=env_value
#
# So the regular expression patterns must match the whole string.


# The value on the left is either KEEP or SKIP.  If the value
# is KEEP then if the environment string matches the regular
# expression then the variable is stored. If the value on the left
# is SKIP then if the variable matches it is not stored.

# Order of the list matters.  The first match is used even if a
# later pattern would also match.  The upshot is that special pattern
# matches should appear first and general ones later.

# If the environment string does not match any pattern then it is
# marked as SKIP.


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
