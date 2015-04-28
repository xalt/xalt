#!/bin/bash


########################################################################
# To get this to work please do the following:
#   a) Modify the Site Specific Settings to match your site
#   b) Make sure that this script and the python script
#      "merge_json_files.py" are in the same directory.
#   c) Make sure the module command is defined by using $BASH_ENV
#      or define the module command here.
#   d) Make sure that LMOD_DIR is defined as well (it is defined by $BASH_ENV).
#
# Revisions:
# April 23, 2015:
#  This has just just some minor tweaks from the previous versions:
#  - It will take one argument as the directory where to put reverseMapD, 
#    so it can be called with ./cray_build_rmapT.sh $XALT_ETC_DIR
#    i.e. no longer need manual edit
#  - use $MODULEPATH instead of hard-coded path
#  - remove ModuleA and simplified PrgEnvA
#  - it assumes that $LMOD_DIR is really the prefix for Lmod installation 
#    rather than whatever sets by some lmod_startup_script, so it adds the 
#    path for Spider appropriately as $LMOD_DIR/lmod/lmod/libexec.
#  These simplifies things.  No longer assumes using lmod as tcl module 
#  replacement.
#
########################################################################
#  Site Specific Setting
########################################################################


  BASE_MODULE_PATH=${MODULEPATH}
  RmapDir=$1/reverseMapD
  
  echo "RmapDir=${RmapDir}"

  PrgEnvA=("PrgEnv-cray"  "PrgEnv-gnu" "PrgEnv-intel")

  #########################################################################
  # must define the module command and $LMOD_DIR:
  #########################################################################

  if [ -f "$BASH_ENV" ]; then
    source $BASH_ENV
  fi

########################################################################
#  End Site Specific Setting
########################################################################

if [ ! -d $RmapDir ]; then
  mkdir -p $RmapDir
fi

SCRIPT_DIR=$(cd $(dirname $(readlink -f "$0")) && pwd)
PATH=$SCRIPT_DIR:$LMOD_DIR/lmod/lmod/libexec:$PATH

module unload "${PrgEnvA[@]}" 2> /dev/null
prev=""
for m in "${PrgEnvA[@]}"; do
    sn=$(dirname $m)
    
    module unload $prev  2> /dev/null
    module load $m       2> /dev/null
    prev=$m

    echo -n '-'
    spider --preload -o jsonReverseMapT $BASE_MODULE_PATH >  ${RmapDir}/rmapT_${m}.JSON
    echo -n '*'

done

echo "*"

OLD=$RmapDir/jsonReverseMapT.old.json
NEW=$RmapDir/jsonReverseMapT.new.json
RESULT=$RmapDir/jsonReverseMapT.json

merge_json_files.py ${RmapDir}/rmapT_*.JSON > $NEW
if [ "$?" = 0 ]; then
  chmod 644 $NEW
  if [ -f $RESULT ]; then
    cp -p $RESULT $OLD
  fi
  mv $NEW $RESULT
fi


