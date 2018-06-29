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
# Helper functions used by XALT script ld
########################################################################

########################################################################
#  Global variables:
LD_LIB_PATH=@sys_ld_lib_path@
XALT_DIR=@xalt_dir@
BASENAME=@path_to_basename@

##########################################################################
#  Check command line arguments to see if user has requested tracing
#  This function returns argA and a value for XALT_TRACING
request_tracing()
{
  if [ "${XALT_TRACING:-}" = "yes" -o  "${XALT_TRACING:-}" = "link" ]; then
    export XALT_TRACING=yes
  else
    unset XALT_TRACING
  fi

  argA=()
  for i in "$@"; do
    if [ "$i" = "--xalt_tracing" ]; then
      export XALT_TRACING=yes
    elif [ "$i" = "-M" -o "$i" = "--print-map" -o "$i" = "--print-output-format" -o "$i" = "--print-memory-usage" ]; then
      :
    else
      argA+=("$i")
    fi
  done
  tracing_msg "XALT Tracing Activated for version: @git@"
}

##########################################################################
#  Print message when tracing

tracing_msg()
{
  if [ "${XALT_TRACING:-}" = "yes" ]; then
    builtin echo ""   1>&2
    builtin echo `date "+%H:%M:%S"` "$@" 1>&2
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

    check=0
    for exe in $(type -p -a $my_name); do
      # Find the next executable down the line after the origin, then check. 
      # Needed for the case where there are multiple wrappers. 
      if [ $exe == $my_path ]; then
        check=1
      fi
      if [ $check == 1 ]; then
        # Now check to see if this choice has the magic string in the first 5 lines.
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
    check=0
    for dir in $PATH; do
      exe="$dir/$my_name"
      if [ $exe == $my_path ]; then
        check=1
      fi
      if [ $check == 1 -a -x $exe ]; then
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
