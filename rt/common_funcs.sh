#!/bin/bash
# -*- shell-script -*-

initialize()
{
  unset LD_PRELOAD
  unset XALT_TRANSMISSION_STYLE
  export XALT_EXECUTABLE_TRACKING=no

  ORIG_HOME=`(cd $HOME; /bin/pwd)`
  HOME=`/bin/pwd`
  numStep=0
  COUNT=0

  module unload xalt

  rm -f  _stderr.* _stdout.* out.* err.* 
  rm -rf .xalt.d syslog.* reverseMapD
  rm -f $projectDir/src/*.o $projectDir/src/*.d $projectDir/libuuid/src/*.o
  XALT_EPOCH_T0=$(python3 $projectDir/rt/xalt_epoch.py)

}

displayThis()
{
  XALT_EPOCH_T1=$(LD_PRELOAD= $outputDir/XALT/xalt/xalt/libexec/xalt_epoch "$XALT_EPOCH_T0")
  echo 
  echo "#==========================================================#"
  echo "  ""$@"
  echo "  ($XALT_EPOCH_T1)"
  echo "#==========================================================#"
}

buildRmapT()
{
  echo "<build xalt_rmapT.json and jsonReverseMapT.json files>"
  mkdir reverseMapD
  MPATH=${LMOD_DEFAULT_MODULEPATH-$MODULEPATH}
  $LMOD_DIR/spider -o xalt_rmapT      $MPATH > $outputDir/reverseMapD/xalt_rmapT.json
  $LMOD_DIR/spider -o jsonReverseMapT $MPATH > $outputDir/reverseMapD/jsonReverseMapT.json
  echo "<finish>"
}

installXALT()
{
  rm -rf XALT build
  mkdir build

  (cd build; echo "<configure>";$projectDir/configure --prefix $outputDir/XALT --with-etcDir=$outputDir --with-config=$projectDir/Config/rtm_config.py "$@" ; \
  echo "<make>"; make OPTLVL="-g -O0" install ;  )
  cp $projectDir/test/check_entries_db.py XALT/xalt/xalt/sbin
  PATH=$outputDir/XALT/xalt/xalt/bin:$outputDir/XALT/xalt/xalt/sbin:$PATH;
  echo "<end make>"
}

installDB()
{
  echo "<create db>"
  DBNAME=testxalt
  DB_CONF_FN=testxalt_db.conf
  DBUSER=xaltTest
  PASSWD=kutwgbh

  conf_create.py     --dbhost localhost --dbuser $DBUSER \
                     --passwd $PASSWD   --dbname $DBNAME
  echo "<create new DB>"
  createDB.py        --drop --confFn $DB_CONF_FN
  rm -f xalt_db.conf 
  ln -s testxalt_db.conf xalt_db.conf 
  echo "<end create db>"
}

runMe ()
{
   COUNT=$(($COUNT + 1))
   numStep=$(($numStep+1))
   NUM=$(printf "%02d" $numStep)
   echo "===========================" >  _stderr.$NUM
   echo "step $COUNT"                 >> _stderr.$NUM
   echo "$@"                          >> _stderr.$NUM
   echo "===========================" >> _stderr.$NUM

   echo "===========================" >  _stdout.$NUM
   echo "step $COUNT"                 >> _stdout.$NUM
   echo "$@"                          >> _stdout.$NUM
   echo "===========================" >> _stdout.$NUM

   numStep=$(($numStep+1))
   NUM=$(printf "%02d" $numStep)
   "$@" > _stdout.$NUM 2>> _stderr.$NUM
}
