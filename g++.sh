#!/bin/bash

export LD_LIBRARY_PATH=/opt/apps/gcc/4.9.1/lib64:/opt/apps/gcc/4.9.1/lib

/opt/apps/gcc/4.9.1/bin/g++ "$@"

