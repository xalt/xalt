#!/bin/bash
# -*- shell-script -*-

initialize()
{
  unset LD_PRELOAD
  export XALT_EXECUTABLE_TRACKING=no

  ORIG_HOME=`(cd $HOME; /bin/pwd)`
  HOME=`/bin/pwd`
  numStep=0
  COUNT=0

  rm -f  _stderr.* _stdout.* out.* err.* 
  rm -rf .xalt.d syslog.log reverseMapD

}

buildRmapT()
{
  echo "<build xalt_rmapT.json and jsonReverseMapT.json files>"
  mkdir reverseMapD
  $LMOD_DIR/spider -o xalt_rmapT $LMOD_DEFAULT_MODULEPATH > $outputDir/reverseMapD/xalt_rmapT.json
  $LMOD_DIR/spider -o jsonReverseMapT $LMOD_DEFAULT_MODULEPATH > $outputDir/reverseMapD/jsonReverseMapT.json
  echo "<finish>"
}

installXALT()
{
  rm -rf XALT build
  mkdir build
  (cd build; echo "<configure>";$projectDir/configure --prefix $outputDir/XALT --with-etcDir=$outputDir --with-syshostConfig=nth_name:2 --with-config=$projectDir/Config/rtm_config.py >& /dev/null ; \
  echo "<make>"; make -f makefile PATH_TO_SRC=$projectDir install >& /dev/null;  )
  cp $projectDir/src/removeDataBase.py    XALT/sbin
  cp $projectDir/test/check_entries_db.py XALT/sbin
  PATH=$outputDir/XALT/bin:$outputDir/XALT/sbin:$PATH;
  echo "<end make>"
}

installDB()
{
  echo "<create db>"
  DBNAME=testxalt
  DBUSER=xaltTest
  PASSWD=kutwgbh

  conf_create.py     --dbhost localhost --dbuser $DBUSER \
                     --passwd $PASSWD   --dbname $DBNAME
  removeDataBase.py  --dbname $DBNAME  > /dev/null 2>&1
  createDB.py        --dbname $DBNAME
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
