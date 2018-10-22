## Synopsis

XALT is a lightweight software tool for any Linux cluster,
workstation, or high-end supercomputer to track executable information
and linkage of static shared and dynamically linked libraries. When
the code is executed, wrappers intercept both the GNU linker (ld) and
code launcher (like mpirun, aprun, or ibrun) to capture linkage
information and environmental variables.

By surveying usage, administrators can use XALT to flag jobs that
require the attention of support staff and deliver alerts to users
pinpointing the causes of problems.  The metrics produced as part of
XALT's output data may be used to improve training, documentation, and
research. Also, by tracking shared libraries required at link time,
and then again at run time, an administrator can track the use of the
shared libraries by their user community and detect when changes have
occurred over time that might point to performance differences or a
hacked SSL library.

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

## Commercial Support
Ellexus is providing commercial support.
See https://www.ellexus.com/products/xalt-tracking-job-level-activity-supercomputers if interested.


## Documentation
Documentation is provided at http://xalt.readthedocs.org as well as
doc/XALTDesignandInstallationManual-0.5.pdf.

## Installation and Use of XALT

For quick installation instructions, see [QUICK_START.TXT](QUICK_START.TXT).

[The XALT Design and Installation Manual-0.5](my_docs/XALTDesignandInstallationManual-0.5.pdf) discusses the
prerequisites for installation, decisions and considerations regarding
database setup, XALT testing, other transmission methods, examples,
and knowns issues.

[The XALT Users Manual-0.5](my_docs/XALTUsersManual-0.5.pdf) describes how
XALT is used in practice with further discusssion of database options,
data transmission, creation of reverse maps, and examples with
accompanying results for database queries.


## ChangeLog

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


## Known Issues and Current Solutions


1. Because the Cray compile wrapper uses a binutils linker, the Cray
environment variable ALT_LINKER in the XALT modulefile must be set to
point to the XALT wrapper.

Known issues are detailed in the [XALT Design and Installation Manual-0.5](my_docs/XALTDesignandInstallationManual-0.5.pdf).


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
