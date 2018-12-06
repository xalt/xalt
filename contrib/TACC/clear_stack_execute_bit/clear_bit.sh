#!/bin/bash -xv
# -*- shell-script -*-

syshost=$1

prgm=$2
if [ -z "$pgrm" -o ! -x "$prgm" -o ! -w "$prgm" ] ; then
    return 0
fi

result=$(execstack $prgm)
if [[ $result =~ ^X ]]; then
    host=$(xalt_extract_record $prgm | grep Build_Syshost | sed -e 's/Build_Syshost\s*//')
    if [[ $host =~ $syshost ]]; then
	execstack -c $prgm
    fi
fi

