#!/bin/bash
# -*- shell-script -*-

initialize()
{
  unset LD_PRELOAD
  export XALT_EXECUTABLE_TRACKING=no
  PATH=$outputDir/XALT/bin:$outputDir/XALT/sbin:$PATH;

  ORIG_HOME=`(cd $HOME; /bin/pwd)`
  HOME=`/bin/pwd`
  numStep=0
  COUNT=0

  rm -f  _stderr.* _stdout.* out.* err.* 
  rm -rf .xalt.d syslog.log

}
buildRmapT()
{
  echo "<build xalt_rmapT.json and jsonReverseMapT.json files>"
  rm -rf reverseMapD
  mkdir  reverseMapD
  MPATH=${LMOD_DEFAULT_MODULEPATH-$MODULEPATH}
  $LMOD_DIR/spider -o xalt_rmapT      $MPATH > $outputDir/reverseMapD/xalt_rmapT.json
  $LMOD_DIR/spider -o jsonReverseMapT $MPATH > $outputDir/reverseMapD/jsonReverseMapT.json
  echo "<finish>"
}

installXALT()
{
  rm -rf XALT build
  mkdir build
  (cd build; echo "<configure>";$projectDir/configure --prefix $outputDir/XALT ; \
  echo "<make>"; make install ;  )
  cp $projectDir/src/removeDataBase.py    XALT/sbin
  cp $projectDir/test/check_entries_db.py XALT/sbin
  PATH=$outputDir/XALT/bin:$outputDir/XALT/sbin:$PATH;
  echo "<end make>"
}

installDB()
{
  DBNAME=testxalt
  DBUSER=xaltTest
  PASSWD=kutwgbh

  conf_create.py     --dbhost localhost --dbuser $DBUSER \
                     --passwd $PASSWD   --dbname $DBNAME
  echo "<remove old DB>"  1>&2
  removeDataBase.py  --dbname $DBNAME  > /dev/null 2>&1
  echo "<create DB>"      1>&2
  createDB.py        --dbname $DBNAME
  echo "<done instalDB>"  1>&2
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
