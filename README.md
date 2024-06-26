![XALT Logo](https://github.com/xalt/xalt/raw/main/logos/XALT_Sticker.png)

[![Documentation Status](https://readthedocs.org/projects/xalt/badge/?version=latest)](https://xalt.readthedocs.io/en/latest/?badge=latest)
## Synopsis

XALT is a lightweight software tool for any Linux cluster,
workstation, or high-end supercomputer to track executable information
and linkage of static shared and dynamically linked libraries. When
the code is executed, wrappers intercept both GNU linker (ld) to capture
linkage information and environmental variables.

## Creators

Dr. Mark Fahey  
Dr. Robert McLay

## Motivation

We wanted to be able to answer the questions: what software do
researchers actually use on high-end computers, and how successful are
they in their efforts to use it?  With the information that xalt
collects, high-end computer administrators can answer our questions by
tracking continuous job-level information to learn what products
researchers do and do not need.  Drilling down to data driven usage
statistics helps stakeholders conduct business in a more efficient,
effective, and systematic way.


## Documentation

* Documentation:   https://xalt.readthedocs.org
* GitHub:          https://github.com/xalt/xalt


## Lmod Mailing list

* mailto:xalt-users@lists.sourceforge.net.

Please go to https://sourceforge.net/projects/xalt/lists/xalt-users to join.



## ChangeLog

### XALT 3.0

Features/Bug Fixes:

   1. Better support for containers including containers based on Alpine.

   2. Use of CRC to protect when UUID are not unique.

   3. Pre-ingestion filtering added.

   $. Compiler support for rust, nim, and chapel

   6. Support for ARM processors running Linux.

   7. Filtering based on command line arguments.
   
### XALT 2.10:

Features/Bug:

   1. It copies necessary libraries to <INSTALL_DIR>/<xalt-version>/lib64

   2. It has remove its dependency on zlib to better support containers. The "syslog" transmission style now send the json string directly. (It is still broken into blocks.)

   3. The XALT DB now contains a column for containers usage/non-usage.
   
   4. The env. var. XALT_TRACING now support a "run" value so that only executions that pass the exec path test produce any output.

   5. XALT has now greatly reduced the number of system calls it makes. Only the curl transmission style requires a system call.

   6. Fixed bug where the XALT watermark was ignored (introduced in XALT 2.9.6) 

### XALT 2.8:

Features/Bug:

   1. xalt_syslog_to_db.py and xalt_file_to_db.py support --u2acct that allows sites to specify a default charge account string when it is not specify in the job script. 

   2. Better support for using XALT_FILE_PREFIX.  Use uuid hash value to chose the 000 to upto a prime number-1 (prime=997 (default)) directory when using XALT_FILE_PREFIX.

   3. Separate Site controllable filter for MPI executables for all MPI tasks < MPI_ALWAYS_RECORD.

   4. Stopping building libuuid. Instead copy libuuid.so from system to XALT's lib64 directory for Container support.  Similar support for other libraries.

   5. Site can control whether the command line is captured or not.

   6. Support curl transmission.


### XALT 2.7:

Features/Bug:

   1. Many bug fixes in the configure process.

   2. Now reads /proc/$PID/maps instead of running ldd and avoids a system call.

   3. Now uses the vendor note to hold the XALT watermark thus avoiding a system call
      to extract this information.

   4. A rpm spec file is provided.

   5. Programs with spaces and quotes in the name of the file and/or directories are now
      safely handled.

   6. The build directory (i.e. the directory where ld was run) is included in the XALT watermark.

   7. There is a new output directory structure when using XALT_FILE_PREFIX that avoids using
      $USER in the directory structure. A new script is provided to create the structure.

### XALT 2.6:

Features/Bug:

   1. All python scripts used to build XALT and load the database are now
      python2/python3 compatible.

   2. Update documentation on how to create the database.

   3. Fix bug where the @git@ string was replaced in the wrong place in Makefile.in

   4. PKGS programs (like R, Python) now use sample rules like all other scalar programs.

### XALT 2.5:

Features:
   
   1. A static version of libuuid.a is now part of XALT.

   2. XALT no longer hard-codes paths at configure time. Instead
      it used --with-systemPath=/usr/bin:/bin to control where XALT 
      looks for executables such as basename

   3. XALT now uses the NVML base library from DCGM to optionally track
      GPU usage.

   4. XALT can be used to track executions in a Singularity container.

   5. XALT now catches signals such as SIGSEGV, etc and re-raises them to
      record an end record.

   6. XALT now treats programs like Python, R and MATLAB just like any other
      program which simplifies scalar sampling rules.

Bug Fix:

   1. Modified the generated assembly code to not set GNU Stack bit.


### XALT 2.4:

Bug Fix:

   1. Support for tracking GPU usage with the DCGM library from NVIDIA.

   2. XALT by default only uses LD_PRELOAD to track execution.
   
### XALT 2.3:

Bug Fix:

   1. Fix bug where XALT would miss libraries that depended on LD_LIBRARY_PATH.
   

### XALT 2.2:

Features:

   1. XALT registers a signal handler so that programs that seg-faults, fpe
      and other signals will generate an end record.

   2. XALT now optional supports detecting GPU usage through the NVIDIA DCGM library.

   3. Direct to database option has been removed.

### XALT 2.1:

Features:

   1. XALT can now tracks both MPI and non-MPI programs subject to site controlled filtering


## Citation

To cite XALT, please use the following:

Agrawal, K., Fahey, M. R., McLay, R., & James, D. (2014). User Environment Tracking and Problem Detection with XALT. In Proceedings of the First International Workshop on HPC User Support Tools (pp. 32–40). Piscataway, NJ, USA: IEEE Press. http://doi.org/10.1109/HUST.2014.6

## Dataset

As part of the creation of XALT, data collected at the Texas Advanced
Computer Center using XALT will be released for public use.
Information about the dataset and links to the data may be found in
the University of Texas TexasScholarWorks repository:

  XALT data: http://web.corral.tacc.utexas.edu/XALT/  
  metadata: http://repositories.lib.utexas.edu/handle/2152/30535

To cite XALT data, please use the following:   

McLay, Robert & Fahey, Mark R. (2015). Understanding the Software Needs of High End Computer Users with XALT. Texas Advanced Computing Center. http://dx.doi.org/10.15781/T2PP4P

## Copyright

XALT: A tool that tracks users jobs and environments on a cluster.  
Copyright (C) 2013-2015 University of Texas at Austin  
Copyright (C) 2013-2015 University of Tennessee

## License

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of
the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser  General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA 02111-1307 USA
