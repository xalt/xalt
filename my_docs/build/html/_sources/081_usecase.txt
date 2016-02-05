Workflow Case Studies
=====================

Queries to the database may be generated to create routine reports to show usage or libraries and identify changes in use over time to support administrative decisions. 

Beyond administrative metrics, tracking with XALT can expose performance issues, which can be fixed

:ref:`programusage`

.. _programusage: 

Program Usage
^^^^^^^^^^^^^

The first case study shows the workflow for discovering a buggy library on darter.

::

	mysql> SELECT link_program, build_syshost, count(*) 
	FROM xalt_link
	GROUP BY link_program, build_syshost;
	
RESULT HERE

::

	mysql> SELECT xalt_link.*
	FROM xalt_link 
	WHERE build_user LIKE '%faheymir%' AND
	exec_path LIKE '%hyperslab%';

RESULT HERE

::

	mysql> SELECT object_path, timestamp 
	FROM xalt_object, join_link_object
	WHERE join_link_object.link_id="4" AND
	join_link_object.obj_id=xalt_object.obj_id;