#!/bin/bash
#-*- shell-script -*-
#
#  Git Version: @git@

#-----------------------------------------------------------------------
# XALT: A tool that tracks users jobs and environments on a cluster.
# Copyright (C) 2013-2014 University of Texas at Austin
# Copyright (C) 2013-2014 University of Tennessee
# 
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of 
# the License, or (at your option) any later version. 
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser  General Public License for more details. 
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA
#-----------------------------------------------------------------------

########################################################################
# Helper functions used by XALT scripts such as ld, ibrun, aprun, etc.
########################################################################

########################################################################
#  Global variables:
XALT_DIR=@xalt_dir@
RUN_SUBMIT=$XALT_DIR/libexec/xalt_run_submission.py
UUIDGEN=@path_to_uuidgen@
WORKING_PYTHON=$XALT_DIR/libexec/xalt_working_python.py
EPOCH=$XALT_DIR/libexec/xalt_epoch.py
BASENAME=@path_to_basename@

########################################################################
# Search for the command  and make sure that you don't find this one.
# We use "type -p -a" instead of searching the path.  Since bash should
# have search and cached all the executable in the path.  This should be
# faster then walking the path.  But only if this script is treated as a
# bash script.
#
# Note: There may be multiple copies of this script in the path.
#       We ignore any that are part of XALT.

# Returns full path to real command.

find_real_command()
{
  local my_path=$1
  local exec_x=$2
  
  local my_name=$($BASENAME $my_path)
  local head="@head@"
  local grep="@grep@"
  local my_cmd="unknown"

  if [ -x "$exec_x" ]; then
    # if $exec_x is exists and is executable then use it
    # This is typically used by ld and when points /usr/bin/ld.x
    my_cmd=$exec_x
  elif [ -n "$BASH_VERSION" ]; then
    # If this is a bash script (and not a bash script in sh mode) then
    # use type to list all possible "ld" or "ibrun".  

    for exe in $(type -p -a $my_name); do
      if [ $exe != $my_path ]; then
        #
        # If the executable is not the same the same as the origin then it is a possible
        # choice.  Now check to see if this choice has the magic string in the first 5 lines.
        # Only if it doesn't contain the magic string then $exe is MY_CMD.
	if ! $head -n 5 $exe | $grep -q "MAGIC_STRING__XALT__XALT__MAGIC_STRING"; then
          my_cmd=$exe
	  break
        fi
      fi
    done
  else
    ###################################################################
    # If this script is not treated as a bash script then do this the
    # old fashion way. Otherwise the logic is the same as above.
    OLD_IFS=$IFS
    IFS=:
    for dir in $PATH; do
      exe="$dir/$my_name"
      if [ $exe != $my_path -a -x $exe ]; then
	if ! $head -n 5 $exe | $grep -q "MAGIC_STRING__XALT__XALT__MAGIC_STRING"; then
          my_cmd=$exe
	  break
        fi
      fi
    done
    IFS=$OLD_IFS
  fi
  builtin echo $my_cmd
}

########################################################################
# Make sure that the python setup is valid. This returns a valid python
# or "broken" if no working python is found
#
# The logic is as follows. Check for at each step.  If pass then stop
# checking, otherwise continue.  
#   1. Check for working python with no changes
#   2. Check for working python with PYTHONHOME unset
#   3. Check for working python with PYTHONPATH unset
#      and system python.
#   4. Report python broken.
#
# Note that the xalt_working_program.py just prints "GOOD" if it works.

find_working_python()
{
  py_home_orig=$PYTHONHOME
  py_path_orig=$PYTHONPATH
  MY_PYTHON=python
  XALT_DIR=@xalt_dir@
  WORKING_PYTHON=$XALT_DIR/libexec/xalt_working_python.py
  WORKING=$($MY_PYTHON $WORKING_PYTHON 2> /dev/null)
  if [ "$WORKING" != "GOOD" ]; then
    unset PYTHONHOME
    WORKING=$($MY_PYTHON $WORKING_PYTHON 2> /dev/null)
    if [ "$WORKING" != "GOOD" ]; then
      unset PYTHONPATH
      MY_PYTHON=@python@
      WORKING=$($MY_PYTHON $WORKING_PYTHON 2> /dev/null)
      if [ "$WORKING" != "GOOD" ]; then
	MY_PYTHON="broken"
      fi
    fi
  fi
}

########################################################################
# Run the real command and wrap the call to real program with
# xalt_run_submission.py

run_real_command()
{
  # Strip leading arguments off of command line
  # This will leave "$@" to be the same as the original in
  # user's call to the wrapper.

  FIND_EXEC_PRGM=$1
  shift
  MY_PYTHON=$1
  shift
  NTASKS=$1
  shift

  # Give NTASKS an illegal value if it wasn't set.
  if [ -z "$NTASKS" ]; then
    NTASKS=-1
  fi

  # Build the filename for the results.
  RUN_UUID=`$UUIDGEN`
  DATESTR=`date +%Y_%m_%d_%H_%M_%S`
  SYSHOST=$($MY_PYTHON $XALT_DIR/site/xalt_syshost.py)
  runFn=$HOME/.xalt.d/run.${SYSHOST}.${DATESTR}.$RUN_UUID.json

  
  # Find the user executable by walking the original command line.
  EXEC="unknown"
  if [ "$FIND_EXEC_PRGM" != "unknown" -a -x "$FIND_EXEC_PRGM" ]; then
    EXEC=$($MY_PYTHON $FIND_EXEC_PRGM "$@")
  fi

  # Record the job record at the start of the job.  This way if the job
  # doesn't complete there will be a record of the job.
  sTime=$($MY_PYTHON $EPOCH)
  $MY_PYTHON $RUN_SUBMIT --ntasks "$NTASKS" --start "$sTime" --end 0        --fn "$runFn" --run_uuid "$RUN_UUID" --syshost "$SYSHOST" -- "$EXEC"



  status=0
  if [ -z "$testMe" ]; then

    # restore python state to what the user originally had
    py_home_xalt=$PYTHONHOME
    py_path_xalt=$PYTHONPATH
    export PYTHONPATH=$PY_path_orig
    [ -n "$PY_HOME_ORIG" ] && export PYTHONHOME=$py_home_orig

    # Run the real command and save the status
    $MY_CMD "$@"
    status="$?"

    # return python state back to XALT
    unset  PYTHONHOME
    [ -n "$PY_HOME_XALT" ] && export PYTHONHOME=$py_home_xalt
    export PYTHONPATH=$py_path_xalt
  fi

  # Record the job record at the end of the job.
  eTime=$($MY_PYTHON $EPOCH)
  $MY_PYTHON $RUN_SUBMIT --ntasks "$NTASKS" --start "$sTime" --end "$eTime" --fn "$runFn" --run_uuid "$RUN_UUID" --syshost "$SYSHOST" -- "$EXEC"

  #----------------------------------------------------------------------
  # The $status variable is used to report the exit status of $MY_CMD"
}
