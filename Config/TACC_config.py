# This is the config file for specifying tables necessary to configure XALT:

# The patterns listed here are the hosts that can track executable with XALT.
# Typical usage is that compute nodes track executable with XALT while login
# nodes do not.

import sys

# Note that linking an executable is everywhere and is independent of
# hostname_patterns

hostname_patterns = [
  ['KEEP', '^c[0-9][0-9][0-9]-[0-9][0-9][0-9]\..*']
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
    ['SPSR',  r'.*\/R'],
    ['SPSR',  r'.*\/MATLAB'],
    ['KEEP',  r'^\/usr\/bin\/cp'],
    ['KEEP',  r'^\/usr\/bin\/mv'],
    ['KEEP',  r'^\/usr\/bin\/gawk'],
    ['KEEP',  r'^\/usr\/bin\/sed'],
    ['KEEP',  r'^\/usr\/bin\/perl'],
    ['KEEP',  r'^\/usr\/bin\/python'],
    ['KEEP',  r'^\/usr\/bin\/grep'],
    ['KEEP',  r'^\/usr\/bin\/bzip2'],
    ['KEEP',  r'^\/usr\/bin\/gzip'],
    ['KEEP',  r'^\/usr\/bin\/tar'],
    ['KEEP',  r'^\/bin\/cp'],
    ['KEEP',  r'^\/bin\/mv'],
    ['KEEP',  r'^\/bin\/gawk'],
    ['KEEP',  r'^\/bin\/sed'],
    ['KEEP',  r'^\/bin\/perl'],
    ['KEEP',  r'^\/bin\/python'],
    ['KEEP',  r'^\/bin\/grep'],
    ['KEEP',  r'^\/bin\/bzip2'],
    ['KEEP',  r'^\/bin\/gzip'],
    ['KEEP',  r'^\/bin\/tar'],
    ['SPSR',  r'.*\/python[0-9.]*'],
    ['SKIP',  r'^\/usr\/.*'],
    ['SKIP',  r'^\/sbin\/.*'],
    ['SKIP',  r'^\/bin\/.*'],
    ['SKIP',  r'^\/etc\/.*'],
    ['SKIP',  r'^\/root\/.*'],
    ['SKIP',  r'^\/opt\/intel\/.*'],
    ['SKIP',  r'^\/opt\/apps\/intel\/.*'],
    ['SKIP',  r'^\/opt\/apps\/cuda\/.*'],
    ['SKIP',  r'^\/opt\/apps\/lua\/.*'],
    ['SKIP',  r'^\/opt\/apps\/lmod\/.*'],
    ['SKIP',  r'^\/opt\/apps\/shell_startup_debug\/.*'],
    ['SKIP',  r'.*\/l\/pkg\/xalt\/'],
    ['SKIP',  r'.*\/l\/pkg\/lua\/'],
    ['SKIP',  r'.*\/l\/pkg\/lmod\/'],
    ['SKIP',  r'^\/opt\/apps\/xalt\/.*'],
    ['SKIP',  r'^\/opt\/apps\/git\/.*'],
    ['SKIP',  r'^\/opt\/apps\/cmake\/.*'],
    ['SKIP',  r'^\/opt\/apps\/autotools\/.*'],
    ['SKIP',  r'^\/opt\/apps\/intel[0-9][0-9_]*\/mvapich2\/.*'],
    ['SKIP',  r'^\/opt\/apps\/intel[0-9][0-9_]*\/impi\/.*'],
    ['SKIP',  r'^\/opt\/apps\/gcc[0-9][0-9_]*\/mvapich2\/.*'],
    ['SKIP',  r'^\/opt\/apps\/gcc\/.*'],
    ['SKIP',  r'.*\/git'],
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
    ['SKIP',  r'.*\/xalt_extract_record\.x'],
    ['SKIP',  r'.*\/xalt_configuration_report\.x'],
    ['SKIP',  r'.*\/xalt_syshost'],
    ['SKIP',  r'.*\/xalt_record_pkg']
  ]

#------------------------------------------------------------
# XALT samples non-mpi executions, but not SPSR programs,
# based on this table. All mpi executions are sampled at 100%
# when there are two more tasks.
#
# The array of array used by interval_array has the following
# structure:
#
#   interval_array = [
#                     [ t_0,     probability_0],
#                     [ t_1,     probability_1],
#                     ...
#                     [ t_n,     probability_n],
#                     [ 1.0e308, 1.0],
#
#
# The first number is the left edge of the time range.  The
# second number is the probability of being sampled. Where a
# probability of 1.0 means a 100% chance of being recorded and a
# value of 0.01 means a 1% chance of being recorded.
#
# So a table that looks like this:
#     interval_array = [
#                       [ 0.0,                0.0001 ],
#                       [ 300.0,              0.01   ],
#                       [ 600.0,              1.0    ],
#                       [ sys.float_info.max, 1.0    ]
#     ]
#
# would say that program with execution time that is between
# 0.0 and 300.0 seconds has a 0.01% chance of being recorded.
# Execution times between 300.0 and 600.0 seconds have a 1%
# chance of being recorded and and programs that take longer
# than 600 seconds will always be recorded.
#
# The absolute minimum table would look like:
#
#     interval_array = [
#                       [ 0.0,                1.0 ],
#                       [ sys.float_info.max, 1.0 ]
#     ]
#
# which says to record every scalar (non-mpi) program no matter
# the execution time.
#
# Note that scalar execution only uses this table IFF
# $XALT_SCALAR_AND_SPSR_SAMPLING equals yes

interval_array = [
    [ 0.0,                0.0001 ],
    [ 300.0,              0.01   ],
    [ 600.0,              1.0    ],
    [ sys.float_info.max, 1.0    ]
]

#------------------------------------------------------------
# XALT samples SPSR programs at one rate no matter the runtime.
# SPSR programs are programs like R and python where the internal
# package use is tracked as well.
#
# By default SPSR programs are sampled at a 100% rate but sites can
# override this by this configuration file.

# Note that SPSR execution only uses this table IFF
# $XALT_SCALAR_AND_SPSR_SAMPLING equals yes
#
# Also note that this value can be overridden. For example the
# following setting:
#
#     export XALT_SPSR_SAMPING_RATE=0.02
#
# would change the SPSR sampling rate to 2%.
#

SPSR_sampling = 0.01  # == 1%

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
    [ 'KEEP', r'^MKL.*'],
    [ 'KEEP', r'^MV2_.*'],
    [ 'KEEP', r'^OFFLOAD.*'],
    [ 'KEEP', r'^OMP.*'],
    [ 'KEEP', r'^PATH=.*'],
    [ 'KEEP', r'^PYTHON.*'],
    [ 'KEEP', r'^R_.*'],
    [ 'KEEP', r'^TACC_AFFINITY_ENABLED=.*'],
    [ 'KEEP', r'^LAUNCHER_TSK_ID=.*'],
    [ 'KEEP', r'^PYLAUNCHER_ENABLED=.*'],
    [ 'KEEP', r'^_LMFILES_=.*'],
  ]

