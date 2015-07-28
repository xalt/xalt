#!/bin/bash
# -*- shell-script -*-

if [ -z "$XALT_ACTIVE" ]; then
    /usr/bin/ld.x "$@"
fi

/opt/apps/xalt/xalt/bin/ld "$@"
