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
collects, high-end computer administrators can answer our questons by
tracking continuous job-level information to learn what products
researchers do and do not need.  Drilling down to data driven usage
statistics helps stakeholders conduct business in a more efficient,
effective, and systematic way.


## Documentation
Installation and Use of XALT is provided at https://xalt.readthedocs.io on how to install and use XALT.

## Commercial Support
Ellexus is providing commercial support.
See https://www.ellexus.com/products/xalt-tracking-job-level-activity-supercomputers if interested.

## ChangeLog

### XALT 2.6:

Features/Bug:

   1. All python scripts used to build XALT and load the database are now
      python2/python3 compatible.

   2. Update documentation on how to create the database.

   3. Fix bug where the @git@ string was replaced in the wrong place in Makefile.in

### XALT 2.5:

Features:
   
   1. A static version of libuuid.a is now part of XALT.

   2. XALT nolonger hardcodes paths at configure time. Instead
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

##License

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
