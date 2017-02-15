
XALT: Library Tracking Software
=====================================

PURPOSE
^^^^^^^
This guide is written to provide installation instructions and give examples of use for XALT.

OVERVIEW
^^^^^^^^
XALT is a lightweight software tool for any Linux cluster, workstation, or high-end supercomputer to track executable information and linkage of static shared and dynamically linked libraries. When the code is executed, wrappers intercept both the GNU linker (ld) and code launcher (like mpirun, aprun, or ibrun) to capture linkage information and environmental variables.

DOWNLOAD
^^^^^^^^

The most current release of XALT can be found at
https://github.com/Fahey-McLay/xalt.git.

Stable releases of XALT can be found at https://xalt.sf.net

New in Version 2.0+
^^^^^^^^^^^^^^^^^^^^

The big change for version 2.0+ of XALT is the change to linker only.  XALT can now caputure non-MPI jobs, single thread, and open MPI.
With the ability to track additional information comes additional challenges.  Now that XALT tracks every executable, ignoring information will be site dependent.

Installing XALT - Getting Started
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   020_QuickStart
   090_FAQ

Topics covered as part of the Quick Start include:

  :doc:`030_installation` |
  :doc:`040_Database` |
  :doc:`050_testing` |
  :doc:`060_production` |
  :doc:`070_datamining` |
  :doc:`080_usecase`



User Guide for  XALT
^^^^^^^^^^^^^^^^^^^^^
For the complete user manual of XALT, see the `User Manual`_.

.. _User Manual: https://github.com/Fahey-McLay/xalt/blob/master/doc/XALTUsersManual-0.5.pdf



Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
