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
#  Global variables:
XALT_DIR=@xalt_dir@
RUN_SUBMIT=$XALT_DIR/libexec/xalt_run_submission.py
UUIDGEN=@path_to_uuidgen@
WORKING_PYTHON=$XALT_DIR/libexec/xalt_working_python.py
EPOCH=$XALT_DIR/libexec/xalt_epoch.py

########################################################################
#  If the first argument is --part_of_xalt then exit immediately
#  This avoids problems when there are more than one copy of this
#  command in the path.
exit_if_part_of_xalt()
{
  if [ "$1" == --part_of_xalt ]; then
    echo "XALT:YES"
    exit
  fi
}


########################################################################
# Search for the command  and make sure that you don't find this one.
# We use "type -p -a" instead of searching the path.  Since bash should
# have search and cached all the executable in the path.  This should be
# faster then walking the path.  But only if this script is treated as a
# bash script.
#
# Note: There may be multiple copies of this script in the path.
#       We ignore any that are part of XALT.

# Returns full to real command.

find_real_command()
{
  local MY_PATH=$1
  local EXEC_X=$2
  local MY_NAME=$(basename $MY_PATH)

  if [ -x "$EXEC_X" ]; then
    MY_CMD=$EXEC_X
  elif [ -n "$BASH_VERSION" ]; then
    for exe in $(type -p -a $MY_NAME); do
      if [ $exe != $MY_PATH ]; then
        MY_CMD=$exe
        part_of_xalt=$($MY_CMD --part_of_xalt 2> /dev/null)
        if [ "$part_of_xalt" != "XALT:YES" ]; then
  	  break
        fi
      fi
    done
  else
    ###################################################################
    # If this script is not treated as a bash script then do this the
    # old fashion way.

    OLD_IFS=$IFS
    IFS=:
    for dir in $PATH; do
      if [ $dir/$MY_NAME != $MY_PATH -a -x "$dir/$MY_NAME" ]; then
        MY_CMD="$dir/$MY_NAME"
        part_of_xalt=$($MY_CMD --part_of_xalt 2> /dev/null)
        if [ "$part_of_xalt" != "XALT:YES" ]; then
  	  break
        fi
      fi
    done
    IFS=$OLD_IFS
  fi
  echo $MY_CMD
}

########################################################################
# Make sure that the python setup is valid.  If a user makes PYTHONHOME
# point to a different python then if stmt will be triggered.

# returns a valid python or "broken" if no working python is found

find_working_python()
{
  PY_HOME_ORIG=$PYTHONHOME
  PY_PATH_ORIG=$PYTHONPATH
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
  FIND_EXEC=$1
  shift
  MY_PYTHON=$1
  shift
  NTASKS=$1
  shift

  if [ -z "$NTASKS" ]; then
    NTASKS=1
  fi

  RUN_UUID=`$UUIDGEN`
  DATESTR=`date +%Y_%m_%d_%H_%M_%S`
  SYSHOST=$($MY_PYTHON $XALT_DIR/site/xalt_syshost.py)
  runFn=$HOME/.xalt.d/run.${SYSHOST}.${DATESTR}.$RUN_UUID.json

  EXEC="unknown"
  if [ "$FIND_EXEC" != "unknown" -a -x "$FIND_EXEC" ]; then
    EXEC=$($MY_PYTHON $FIND_EXEC "$@")
  fi

  sTime=$($MY_PYTHON $EPOCH)
  $MY_PYTHON $RUN_SUBMIT --ntasks "$NTASKS" --start "$sTime" --end 0        --fn "$runFn" --run_uuid "$RUN_UUID" -- "$EXEC"

  status=0
  if [ -z "$testMe" ]; then
    PY_HOME_XALT=$PYTHONHOME
    PY_PATH_XALT=$PYTHONPATH
    export PYTHONPATH=$PY_PATH_ORIG
    [ -n "$PY_HOME_ORIG" ] && export PYTHONHOME=$PY_HOME_ORIG

    $MY_CMD "$@"
    status="$?"

    unset  PYTHONHOME
    [ -n "$PY_HOME_XALT" ] && export PYTHONHOME=$PY_HOME_XALT
    export PYTHONPATH=$PY_PATH_XALT
  fi

  eTime=$($MY_PYTHON $EPOCH)
  $MY_PYTHON $RUN_SUBMIT --ntasks "$NTASKS" --start "$sTime" --end "$eTime" --fn "$runFn" --run_uuid "$RUN_UUID" -- "$EXEC"
}

  

