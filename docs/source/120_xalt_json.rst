A Description of XALT's json record
-----------------------------------

XALT's ld wrapper and the actively tracked executable by XALT generate
\*.json files transport the data from these activities back to XALT's
database. There are three different types of \*.json records: link, pkg,
and run.

Link \*.json records
^^^^^^^^^^^^^^^^^^^^

The link \*.json records if written to a file will be named as
follows::

    link.<syshost>.<date>.<user>.<uuid>.json

It has contains the following three arrays and one table: The first is
called "function". The function tracking options requires two link
steps.  The extra link step tries to link the executable when the
libraries associated with modules are removed.  If function tracking
is turned on then this array contains the list of function names that
were not satified when the "module" libraries were removed.

For example this might look like for a simple mpi program::

    "function": [
        "MPI_Finalize",
        "MPI_Comm_size",
        "MPI_Init",
        "MPI_Comm_rank"
    ],

The next array is the "linkA" array.  This contains a two entry
array. The first is the path to the library and either a "0" or a sha1
of the library.  Since computing the sha1 of each library turns out to
be time expensive, a zero is there instead of the sha1.  An example
might be::

    "linkA": [
        [
            "/lib/x86_64-linux-gnu/ld-2.27.so",
            "0"
        ],
        [
            "/lib/x86_64-linux-gnu/libc-2.27.so",
            "0"
        ],
        [
            "/opt/apps/gcc-7/mpich/3.2.1/lib/libmpi.so.12.1.1",
            "0"
        ],
    ],

The next array is a copy of the link line arguments.  An example of
this is::

    "link_line": [
        "gcc",
        "-g",
        "-o",
        "hello .mpi",
        "/home/user/mpi_hello_world.c",
        "-I/opt/apps/gcc-7/mpich/3.2.1/include",
        "-L/opt/apps/gcc-7/mpich/3.2.1/lib",
        "-Wl,-rpath",
        "-Wl,/opt/apps/gcc-7/mpich/3.2.1/lib",
        "-Wl,--enable-new-dtags",
        "-lmpi"
    ],

The final part of the link data is a table of key-value pairs that
contains some of the information stored in the "watermark" that all
XALT build executables contain.  Below is an example::

    "resultT": {
        "build_epoch": "1569439459.6736",
        "build_syshost": "stampede2",
        "build_user": "user",
        "exec_path": "/home/user/hello.mpi",
        "hash_id": "b2ba1c4fdc1b709aad58a5c8b78d86dafda08ed9",
        "link_path": "/bin/gcc",
        "link_program": "gcc",
        "uuid": "b0a4caaf-1f05-4caa-af69-1e4ff8bea351",
        "wd": "/home/user/"
    }

The **exec_path** field contains the path to the built executable and wd
contains the current directory where "ld" was invoked.

Pkg \*.json records
^^^^^^^^^^^^^^^^^^^

Programs like R, Python and MATLAB have an "import" or "use" mechanism
to dynamically load packages during execution.  For example R could import
and particular package.  It is possible to hook into R's import
mechanism and capture it for XALT.  These json records are just one
table that looks like::

    {
        "package_name": "bar",
        "package_path": "/A/B/bar/3.0",
        "package_version": "3.0",
        "program": "R",
        "xalt_run_uuid": "1a8bc989-0592-43f4-952c-6b5514ad1a8a"
    }   

Run \*.json records
^^^^^^^^^^^^^^^^^^^

The run json records contain many arrays and tables to capture the
execution performance. The run \*.json records if written to a file
will be named as follows::

    run.<syshost>.<date>.<user>.<start/end>.<uuid>.json

The start/end is either *aaa* for a start record or *zzz* for an end
record. 


The first table is "XALT_measureT".  This table is for internal
development of the XALT package. It captures how long it takes the
**xalt_run_submission** program to build the json record.  The last
number in this table contains the total time for the program to build
the json record. This number may be of general interest.::

    "XALT_measureT": {
        "01_BuildUserT___": 4.6e-05,
        "02_Sha1_exec____": 0.000396,
        "03_BuildEnvT____": 0.000421,
        "04_WalkProcTree_": 0.000219,
        "05_ExtractXALTR_": 1.2e-05,
        "06_ParseProcMaps": 6.5e-05,
        "06_SO_sha1_comp_": 4e-06,
        "07____total_____": 0.001175
    },

The next array is "cmdlineA" contains the user's command line for the
executable. Following standard json rules all utf-8 characters are
converted to \\uXXXX where the XXXX is hex value.::

    "cmdlineA": [
        "hello\u00f1.mpi"
    ],

The next table is the "envT" table.  This contains the filtered
key-value pairs of the program's environment.::

    "envT": {
        "LD_LIBRARY_PATH": "...",
        "LOADEDMODULES": "...",
        "MODULEPATH": "...",
        "PATH": "...",
        "PYTHONPATH": "...",
        "_LMFILES_": "..."
    },


The next value is "hash_id" which is a sha1 sum of the executable.::

    "hash_id": "f1f2510ce9007728218ba39a63fcd5209defb5ca",

The libA array is in the exact same format as the libA array discussed
above for the linkA \*.json record.

The "ptA" in the process tree that the executable ran under.  The
first entry in the parent process of the current executable.  The next
entry is the parent process of the one above it. The process tree
traversed until just before the init or systemd process.

The "userDT" table contains all the numerical values associated with the
process.  These include the start_time, end_time and run_time. Note
that if a record is a start record that the end_time and run_time will
be zero (0.0)::

    "userDT": {
        "Build_Epoch": 1569439459.7851,
        "currentEpoch": 1569439459.851167,
        "end_time": 1569439459.8475,
        "exec_epoch": 1569439459.0,
        "num_cores": 1.0,
        "num_gpus": 0.0,
        "num_nodes": 1.0,
        "num_tasks": 1.0,
        "num_threads": 1.0,
        "probability": 1.0,
        "run_time": 0.0034,
        "start_time": 1569439459.8441
    },

The "userT" table contains the string values associated with the
process.  This includes the current working directory, (cwd) and the
absolute path to the executable (exec_path). Also included is the type
of scheduler that XALT think was used.::

    "userT": {
        "account": "XXXX",
        "currentEpoch": "1569439459.851167",
        "cwd": "/home/user",
        "execModify": "Wed Sep 25 14:24:19 2019",
        "exec_path": "/home/user/hello.mpi",
        "exec_type": "binary",
        "job_id": "12345",
        "queue": "unknown",
        "run_uuid": "8c8d61fb-98c1-44c0-874c-d613312269f4",
        "scheduler": "SLURM",
        "start_date": "Wed Sep 25 14:24:19 2019",
        "submit_host": "unknown",
        "syshost": "stampede2",
        "user": "user"
    },

The final table is the "xaltLinkT" table.  This table will only have
entries if the executable was built under XALT and has an XALT
watermark.  An example is::

    "xaltLinkT": {
        "Build_CWD": "/home/user",
        "Build_Epoch": "1569439459.7851",
        "Build_LMFILES": "...",
        "Build_LOADEDMODULES": "...",
        "Build_OS": "Linux 3.10.0-957.5.1.el7.x86_64",
        "Build_Syshost": "stampede2",
        "Build_UUID": "5468e07b-0c8a-49a1-8010-3c9a11fce259",
        "Build_User": "user",
        "Build_Year": "2019",
        "Build_compiler": "gcc",
        "Build_compilerPath": "/bin/gcc",
        "Build_date": "Wed Sep 25 14:24:19 2019",
        "Build_host": "login1.stampede2.tacc.utexas.edu",
        "XALT_Version": "2.7.9"
    }
