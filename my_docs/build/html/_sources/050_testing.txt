Testing
=======

Setup
^^^^^^

Set transmission style to file for initial testing and load your new XALT modulefile.

To test with multiple MPIs, keep XALT first in the path.


Testing
^^^^^^^

Compile a code and run it in a job with XALT loaded in the job.

There should be two files: link.machine.XXXX and run.machine.XXXX

link.machine*::

  IMAGE

run.machine*::

  IMAGE

:ref:`troubleshooting`

If all output in the files looks right, test the parsing of the data and transmission to the database.
If you have files that have bad data, you can either delete just the bad output files or delete all the files. Create new files until you think everything looks alright.

Now use the sbin/xalt_file_todb.py script::

  IMAGE

:ref:`File to database Issues <troubleshooting2>`

Test directdb by setting XALT_TRANSMISSION_STYLE to directdb eith on command line or in modulefile. This method has the database updated immediately when Id or mpirun wrappers are used.
Even if you don't want to use it, test it at this point to avoid problems later.

.. _troubleshooting:

Common Issues and Troubleshooting
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
note: connect back to this area of the documentation

link.machine and run.machine issues

* Syshost name

    Solution: need to go back and work on xalt_syshost.py

* Num_cores

    Issue: site_pkg or launcher

* Modulefile name

    Issue: reverse map

    Solution:

    check spider output

    make sure to compile a code that uses hdf5 or netcdf or another package that should have a modulefile name.

.. _troubleshooting2:

File to database issues

* Access Issues

    Issue: only certain accounts can insert or access is only granted to hosts

    Solution:

*Simple Summary Report - Bad Entires

    Issue: Couldn't decode file (or syslog)

    Solution: for each bad entry there is a number printed that is the length of the number of characters - this number may be a clue to syslog limit issues.

    Issue: Couldn't insert data into database.

    Solution:

Next Steps - Production
^^^^^^^^^^^^^^^^^^^^^^^
  :doc:`060_production`
