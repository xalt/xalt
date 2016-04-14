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
LD_LIB_PATH=@sys_ld_lib_path@
XALT_DIR=@xalt_dir@
RUN_SUBMIT=$XALT_DIR/libexec/xalt_run_submission.py
UUIDGEN=@uuidgen@
WORKING_PYTHON=$XALT_DIR/libexec/xalt_working_python.py
EPOCH=$XALT_DIR/libexec/xalt_epoch.py
BASENAME=@path_to_basename@
DIRNAME=@path_to_dirname@
PyPATH=/usr/bin:/bin

##########################################################################
#  Check command line arguments to see if user has requested tracing
#  This function returns argA and a value for XALT_TRACING
request_tracing()
{
  if [ -n "${XALT_TRACING:-}" ]; then
    XALT_TRACING="yes"
  fi
  argA=()
  for i in "$@"; do
    if [ "$i" = "--xalt_tracing" ]; then
      XALT_TRACING=yes
    else
      argA+=("$i")
    fi
  done
  if [ -n "${XALT_TRACING:-}" ]; then
    export XALT_TRACING
  fi
  tracing_msg "XALT Tracing Activated for version: @git@"
}

##########################################################################
#  Print message when tracing

tracing_msg()
{
  if [ "${XALT_TRACING:-}" = "yes" ]; then
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
  elif [ -n "${BASH_VERSION:-}" ]; then
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
# Do not mess around.  Use the python and the value of LD_LIBRARY_PATH
# that existed at configure time.  Assume that it is possible that the
# user will do something to mess things up.

# Note that the xalt_working_program.py just prints "GOOD" if it works.

find_working_python()
{
  MY_PYTHON="@python@"
  WORKING=$(LD_LIBRARY_PATH=$LD_LIB_PATH PATH=$PyPATH $MY_PYTHON -E  $WORKING_PYTHON 2> /dev/null)
  if [ "$WORKING" != "GOOD" ]; then
     MY_PYTHON="broken"
  fi

  tracing_msg "find_working_python: Setting MY_PYTHON to $MY_PYTHON"

  if [ "$MY_PYTHON" = "broken" ]; then
    builtin echo "XALT: Error in users' python setup.  Please report this error!"
    $MY_CMD "$@"
    exit $?
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

  FIND_EXEC_PRGM="$1"
  shift
  MY_PYTHON="$1"
  shift

  # Build the filename for the results.
  SYSHOST=$(LD_LIBRARY_PATH=$LD_LIB_PATH PATH=$PyPATH $MY_PYTHON -E $XALT_DIR/site/xalt_syshost_@site_name@.py)
  
  # Find the user executable by walking the original command line.
  EXEC_T="[{\"exec_prog\": \"unknown\", \"ntasks\": 1, \"uuid\": \"$($UUIDGEN)\"} ]"
  if [ "$FIND_EXEC_PRGM" != "unknown" -a -f "$FIND_EXEC_PRGM" ]; then
    EXEC_T=$(LD_LIBRARY_PATH=$LD_LIB_PATH PATH=$PyPATH $MY_PYTHON -E $FIND_EXEC_PRGM "$@")
  fi

  tracing_msg "run_real_command: User's EXEC_T: $EXEC_T"
  
  # Hand it over to the real command if there is exception and return
  if [ "$EXEC_T" == "find_exec_exception" ]; then
    $MY_CMD "$@"
    return $?
  fi

  # Record the run record at the start of the job.  This way if the run
  # doesn't complete there will be a record.

  tracing_msg "run_real_command: XALT Start Record"
  sTime=$(LD_LIBRARY_PATH=$LD_LIB_PATH PATH=$PyPATH $MY_PYTHON -E $EPOCH)
  LD_LIBRARY_PATH=$LD_LIB_PATH PATH=$PyPATH $MY_PYTHON -E $RUN_SUBMIT --start "$sTime" --end 0      --syshost "$SYSHOST" -- "$EXEC_T"

  status=0
  if [ -z "${testMe:-}" ]; then

    tracing_msg "run_real_command: Running: $MY_CMD"
    # Run the real command and save the status
    $MY_CMD "$@"
    status="$?"

  fi

  tracing_msg "run_real_command: XALT End Record"
  # Record the job record at the end of the job.
  eTime=$(LD_LIBRARY_PATH=$LD_LIBR_PATH PATH=$PyPATH $MY_PYTHON -E $EPOCH)
  LD_LIBRARY_PATH=$LD_LIB_PATH PATH=$PyPATH $MY_PYTHON -E $RUN_SUBMIT --start "$sTime" --end "$eTime" --syshost "$SYSHOST" --status $status -- "$EXEC_T"

  #----------------------------------------------------------------------
  # The $status variable is used to report the exit status of $MY_CMD"
}

########################################################################
# Find compiler calling ld to be passed to as arguments to $GEN_LINDATA 
# later (in a subshell)

find_compiler()
{
  COMPILER=$($MY_PYTHON -c "from xalt_util import *; print(extract_compiler())")
  tracing_msg "find_compiler: Setting COMPILER to $COMPILER"
}
