Interception Options
====================

**Intercepting: linkers and launchers**
If you have multiple code launchers and linkers, you must choose a method to intercept them. There are currently three options for wrapping.
	#. Aliasing  
	#. XALT modulefile
	#. Move the launcher
	
**Aliasing: Get in front of the code launcher**

Each alias points to a unique python script. An analysis of the command line gives the name of the name of the original command along with parameters - name of compiler, executable or job file, requested number of processors.


**XALT modulefile**

Create an XALT modulefile that on loading puts the launcher first in the path. 
An example is provided with the xalt.tar file and can be used to help make XALT part of the default environment. 
Issue: you must address any change to the MPI library be a module swap will not keep the XALT wrapper in the path. 
	- Have each MPI modulefile reload the XALT modulefile (or inline the XALT modulefile contents)
	- Use Lmod (:ref:`Lmod`) as the module system which allows one to specify priorities on the PATH setting. Set the XALT PATH priority to ensure MPI modulefile swaps will not be a problem.
	

**Move the Launcher**

Each launcher can be moved by renaming it. This is not our suggested option because it requires modifications to our wrapper. See user guide for an example and additional documentation. 

:doc:`020_QuickStart`

