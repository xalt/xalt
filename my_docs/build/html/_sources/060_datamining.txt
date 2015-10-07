Data Mining
============

Module File Usage
^^^^^^^^^^^^^^^^^
Example::

    mysql>
    SELECT xalt_object.module_name,
    count(xalt_run.date) AS Jobs,
    ROUND(SUM(run_time*num_cores)/3600) as TotalSUs FROM xalt_run,
    xalt_link, join_link_object, xalt_object
    WHERE xalt_run.syshost='darter’
    AND xalt_object.module_name is NOT NULL
    AND xalt_run.uuid = xalt_link.uuid
    AND xalt_link.link_id = join_link_object.link_id
    AND join_link_object.obj_id = xalt_object.obj_id
    AND xalt_run.date >= '2014-11-01’ AND xalt_run.date <= '2014-11-09’
    GROUP BY xalt_object.module_name ORDER BY Jobs DESC;

Result

How to put in images - reference

.. image:: _static/mysql1results.png
    :width: 365px
    :align: left
    :height: 253px
    :alt: alternate text

.. image:: _static/xaltinfo.jpg
