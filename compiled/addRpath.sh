#!/bin/bash
# -*- shell-script -*-

ansA=()

for i in "$@"; do
  case $i in
    -L*)
        path=${i:2}
        ansA+=( "-Wl,-rpath,$path" $i )
        ;;
    *)
        ansA+=( $i )
        ;;
  esac
done


echo "${ansA[@]}"
