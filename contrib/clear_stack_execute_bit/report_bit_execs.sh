#!/bin/bash 
# -*- 

syshost=$1

shift

for prgm in "$@"; do
  if [ -n "$prgm" -a  -r "$prgm" -a  -x "$prgm" -a  -w "$prgm" ] ; then

      result=$(execstack $prgm 2> /dev/null)
      if [[ $result =~ ^X ]]; then
	  host=$(xalt_extract_record $prgm | grep Build_Syshost | sed -e 's/Build_Syshost\s*//')
	  if [[ $host =~ $syshost ]]; then
  	      echo "$prgm"
	  fi
      fi
  fi
done
