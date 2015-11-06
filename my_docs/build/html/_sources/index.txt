
XALT: Library Tracking Software
=====================================

PURPOSE
^^^^^^^
This guide is written to explain the benefits of XALT, installation, and give examples of use.

OVERVIEW
^^^^^^^^
XALT is a lightweight software tool for any Linux cluster, workstation, or high-end supercomputer to track executable information and linkage of static shared and dynamically linked libraries. When the code is executed, wrappers intercept both the GNU linker (ld) and code launcher (like mpirun, aprun, or ibrun) to capture linkage information and environmental variables.

Installing XALT - Getting Started
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   020_QuickStart
   030_FAQ

Topics covered as part of Quick Start include:

  :doc:`010_installation` |
  :doc:`040_testing` |
  :doc:`050_production` |
  :doc:`060_datamining`



User Guide for  XALT
^^^^^^^^^^^^^^^^^^^^^
For the complete user manual of XALT, see the User Manual.

.. toctree::
      :maxdepth: 1

      010_user

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
