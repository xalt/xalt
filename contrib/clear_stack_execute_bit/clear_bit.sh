#!/bin/bash 
# -*- shell-script -*-

syshost=$1

shift

EXECSTACK=/bin/execstack
XER=/opt/apps/xalt/xalt/bin/xalt_extract_record

for prgm in "$@"; do
  if [ -n "$prgm" -a  -r "$prgm" -a  -x "$prgm" -a  -w "$prgm" ] ; then
      result=$($EXECSTACK $prgm 2> /dev/null)
      if [[ $result =~ ^X ]]; then
	  host=$($XER $prgm | grep Build_Syshost | sed -e 's/Build_Syshost\s*//')
	  if [[ $host =~ $syshost ]]; then
  	      execstack -c "$prgm"
	  fi
      fi
  fi
done
