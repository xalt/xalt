# This is the config file for specifying tables necessary to configure XALT:

# The patterns listed here are the hosts that can track executable with XALT.
# Typical usage is that compute nodes track executable with XALT while login
# nodes do not.

import sys

# Note that linking an executable is everywhere and is independent of
# hostname_patterns

hostname_patterns = [
  ['KEEP', r'.*']                    # match  all hosts
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
    ['CUSTOM', r'.*\/python[0-9][^/][^/]*'],
    ['CUSTOM', r'.*\/fakePrgm'],
    ['PKGS',   r'.*\/R'],
    ['PKGS',   r'.*\/test_record_pkg_[0-9][0-9]*'],
    ['PKGS',   r'.*\/get_XALT_env'],
    ['KEEP',   r'^\/usr\/bin\/ddt'],
    ['SKIP',   r'^\/usr\/.*'],
    ['SKIP',   r'^\/sbin\/.*'],
    ['SKIP',   r'^\/bin\/.*'],
    ['SKIP',   r'^\/etc\/.*'],
    ['SKIP',   r'^\/root\/.*'],
    ['SKIP',   r'.*\/.cargo\/bin\/.*'],
    ['SKIP',   r'.*\/gcc'],
    ['SKIP',   r'.*\/g\+\+'],
    ['SKIP',   r'.*\/as'],
    ['SKIP',   r'.*\/ld'],
    ['SKIP',   r'.*\/lto1'],
    ['SKIP',   r'.*\/lto-wrapper'],
    ['SKIP',   r'.*\/cc1plus'],
    ['SKIP',   r'.*\/cc1'],
    ['SKIP',   r'.*\/collect2'],
    ['SKIP',   r'.*\/f951'],
    ['SKIP',   r'.*\/gfortran'],
    ['SKIP',   r'.*\/git'],
    ['SKIP',   r'.*\/icc'],
    ['SKIP',   r'.*\/icpc'],
    ['SKIP',   r'.*\/ifort'],
    ['SKIP',   r'.*\/lua'],
    ['SKIP',   r'.*\/mpiCC'],
    ['SKIP',   r'.*\/mpicc'],
    ['SKIP',   r'.*\/mpicxx'],
    ['SKIP',   r'.*\/mpif77'],
    ['SKIP',   r'.*\/mpif90'],
    ['SKIP',   r'.*\/mpifort'],
    ['SKIP',   r'.*\/mpifc'],
    ['SKIP',   r'.*\/mpigcc'],
    ['SKIP',   r'.*\/mpigxx'],
    ['SKIP',   r'.*\/mpiicc'],
    ['SKIP',   r'.*\/mpiicpc'],
    ['SKIP',   r'.*\/mpiifort'],
    ['SKIP',   r'.*\/mpiexec'],
    ['SKIP',   r'.*\/mpirun'],
    ['SKIP',   r'.*\/mpiexec.hydra'],
    ['SKIP',   r'.*\/hydra_pmi_proxy'],
    ['SKIP',   r'.*\/ompi_info'],
    ['SKIP',   r'.*\/opal_wrapper'],
    ['SKIP',   r'.*\/orterun'],
    ['SKIP',   r'.*\/vtwrapper'],
    ['SKIP',   r'.*\/conftest'],
    ['SKIP',   r'.*\/CMakeTmp\/cmTryCompileExec[0-9][0-9]*'],
    ['SKIP',   r'.*\/CMakeTmp\/cmTC_[a-f0-9][a-f0-9]*'],
]

path_arg_patterns = [
  ['JUMP_1',   r'.*\/python[0-9][^/;][^/;]*;-m'],
  ['SKIP',     r'.*\/python[0-9][^/;][^/;]*;-c'],
  ['SKIP',     r'.*\/python[0-9][^/;][^/;]*;.*\/share\/.*'],
  ['PKGS',     r'.*\/python[0-9][^/;][^/;]*;.*\/data\/.*'],
  ['PKGS',     r'.*\/python[0-9][^/;][^/;]*;'],
  ['SKIP',     r'.*\/fakePrgm;.*\/share\/.*'],
  ['KEEP',     r'.*\/fakePrgm;.*\/data\/.*'],
  ['JUMP_1',   r'.*\/fakePrgm;--opt'],
  ['CONTINUE', r'.*\/fakePrgm;--opt=.*'],
  ['KEEP',     r'.*\/fakePrgm;'],
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
    [ 'KEEP', r'^_LMFILES_=.*']
  ]

#------------------------------------------------------------
# XALT samples almost all  executions (both MPI and scalar) 
# based on this table below.  Note that an MPI execution is where
# the number of tasks is greater than 1.  There is no check to
# see if there are MPI libraries in the executable.  Note that
# the number of tasks are MPI tasks not threads.

# Any time there are a number of short rapid executions these
# have to be sampled. However, there are MPI executions with large
# number of tasks that are always recorded.  This is to allow the
# tracking of long running MPI tasks that never produce an end
# record. By default MPI_ALWAYS_RECORD = 2.  Namely that all MPI tasks are
# recorded.

MPI_ALWAYS_RECORD = 2

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

#interval_array = [
#    [ 0.0,                1.0 ],
#    [ sys.float_info.max, 1.0 ]
#]

#------------------------------------------------------------
# Python pattern for python package tracking

# Note that sys, os, re, and subprocess can not be tracked due to the way that python tracking works.

python_pkg_patterns = [
  { 'k_s' : 'SKIP', 'kind' : 'path', 'patt' : r"^[^/]"      },  # SKIP all built-in packages
  { 'k_s' : 'SKIP', 'kind' : 'name', 'patt' : r"^_"         },  # SKIP names that start with a underscore
  { 'k_s' : 'SKIP', 'kind' : 'name', 'patt' : r".*\."       },  # SKIP all names that are divided with periods: a.b.c
  { 'k_s' : 'KEEP', 'kind' : 'path', 'patt' : r".*/.local/" },  # KEEP all packages installed by users
  { 'k_s' : 'SKIP', 'kind' : 'path', 'patt' : r"/home"      },  # SKIP all other packages in user locations
]

#------------------------------------------------------------
# XALT will also allow for pre-ingestion filtering of *.json
# records.  This array contains a list of path patterns that
# will be filtered.  The 1st column specifies the chance to
# ingest that *.json record.  The 1st line states that there
# is a zero percent chance for recording any executable named
# "foobar" and a 1% chance for recording any executable named
# "BAZ"


pre_ingest_patterns = [
#   percent   path pattern
    [0.0,     r'.*\/foobar'],
    [0.01,    r'.*\/BAZ'],
]

#------------------------------------------------------------
# XALT will allow the filtering of all packages.
# You can SKIP (or KEEP) package based on the name or path.
# Note that you can use pkg_patterns for all patterns including python patterns
# You must match the case of the program (R, python, MATLAB) and the kind (name, path).
# Note that these patterns are flex regex patterns and not python regex patterns.

pkg_patterns = [
  ["SKIP",  r'^R:name:stats'],         # SKIP the R pkg named stats
  ["SKIP",  r'^R:name:base'],          # SKIP the R pkg named base
  ["SKIP",  r'^R:name:methods'],       # SKIP the R pkg named methods
  ["SKIP",  r'^python:name:_.*'],      # SKIP all python name that start with an underscore
  ["SKIP",  r'^python:path:[^/].*'],   # SKIP all python built-in packages
  ["SKIP",  r'^python:path:\/home'],   # SKIP all python package in user locations
]
