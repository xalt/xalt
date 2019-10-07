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
The exec_path field contains the path to the built executable and wd
contains the current directory where "ld" was invoked.

