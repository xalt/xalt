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
DIRNAME=@path_to_dirname@

##########################################################################
#  Check command line arguments to see if user has requested tracing
#  This function returns argA and a value for XALT_TRACING
request_tracing()
{
  if [ -z "$XALT_TRACING" ]; then
    XALT_TRACING="no"
  fi
  argA=()
  for i in "$@"; do
    if [ "$i" = "--xalt_tracing" ]; then
      XALT_TRACING=yes
    else
      argA+=("$i")
    fi
  done
  tracing_msg "XALT Tracing Activated"
}

##########################################################################
#  Print message when tracing

tracing_msg()
{
  if [ "$XALT_TRACING" = "yes" ]; then
    builtin echo ""   1>&2
    builtin echo "$@" 1>&2
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

# This sets the GLOBAL $MY_CMD variable.

find_real_command()
{
  local my_path=$1
  local exec_x=$2
  
  local my_name=$($BASENAME $my_path)
  local head="@head@"
  local grep="@grep@"
  local my_cmd="unknown"

  tracing_msg "find_real_command: Searching for the real: $my_name"

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
    # old fashion way. Otherwise the logic is the same as bash version.
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
  if [ "$my_cmd" = unknown ]; then
    builtin echo "XALT Error: unable to find $my_name"
    false
    exit $?
  fi
  MY_CMD=$my_cmd  
  tracing_msg "find_real_command: found $MY_CMD"
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

  tracing_msg "find_working_python: Setting MY_PYTHON to $MY_PYTHON"

  if [ "$MY_PYTHON" = "broken" ]; then
    builtin echo "XALT: Error in users' python setup.  Please report this error!"
    $MY_CMD "$@"
    exit $?
  fi
}

########################################################################
# Once we have found the xalt commands we don't want to have XALT_BIN
# in the PATH.
remove_xalt_bin_from_path()
{
  local cmd=$1 
  local cmdpath=$(type -p $cmd) 
  local xalt_bin=$($DIRNAME $($READLINK -f $cmdpath))
  PATH=$($MY_PYTHON $XALT_DIR/libexec/xalt_remove_from_path.py $xalt_bin)       
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

  # Build the filename for the results.
  SYSHOST=$($MY_PYTHON $XALT_DIR/site/xalt_syshost.py)
  
  # Find the user executable by walking the original command line.
  EXEC_T="[ {'exec_prog':'unknown', 'ntask'='1'} ]"
  if [ "$FIND_EXEC_PRGM" != "unknown" -a -x "$FIND_EXEC_PRGM" ]; then
    EXEC_T=$($MY_PYTHON $FIND_EXEC_PRGM "$@")
  fi

  tracing_msg "run_real_command: User's EXEC_T: $EXEC_T"

  # Record the run record at the start of the job.  This way if the run
  # doesn't complete there will be a record.

  tracing_msg "run_real_command: XALT Start Record"
  sTime=$($MY_PYTHON $EPOCH)
  UUID_A=$($MY_PYTHON $RUN_SUBMIT --start "$sTime" --end 0 --uuidgen "$UUIDGEN" --syshost "$SYSHOST" -- "$EXEC_T")

  status=0
  if [ -z "$testMe" ]; then

    # restore python state to what the user originally had
    py_home_xalt=$PYTHONHOME
    py_path_xalt=$PYTHONPATH
    export PYTHONPATH=$py_path_orig
    [ -n "$py_home_orig" ] && export PYTHONHOME=$py_home_orig

    tracing_msg "run_real_command: Running: $MY_CMD"
    # Run the real command and save the status
    $MY_CMD "$@"
    status="$?"

    # return python state back to XALT
    unset  PYTHONHOME
    [ -n "$py_home_xalt" ] && export PYTHONHOME=$py_home_xalt
    export PYTHONPATH=$py_path_xalt
  fi

  tracing_msg "run_real_command: XALT End Record"
  # Record the job record at the end of the job.
  eTime=$($MY_PYTHON $EPOCH)
  $MY_PYTHON $RUN_SUBMIT --start "$sTime" --end "$eTime" --uuidA "$UUID_A" --syshost "$SYSHOST" --status $status -- "$EXEC_T"

  #----------------------------------------------------------------------
  # The $status variable is used to report the exit status of $MY_CMD"
}
