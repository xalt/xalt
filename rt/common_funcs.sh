#!/bin/bash
# -*- shell-script -*-

initialize()
{
  unset LD_PRELOAD
  unset XALT_TRANSMISSION_STYLE
  export XALT_EXECUTABLE_TRACKING=no
  module --quiet unload $LMOD_FAMILY_MPI $LMOD_FAMILY_COMPILER
  ORIG_HOME=`(cd $HOME; /bin/pwd)`
  HOME=`/bin/pwd`
  rm -rf $HOME/.local
  ln -s $ORIG_HOME/.local $HOME
  numStep=0
  COUNT=0

  module --quiet unload xalt

  rm -f  _stderr.* _stdout.* out.* err.* leftover.* \#* .\#*
  rm -rf .xalt.d syslog.*
  rm -f $projectDir/src/*.o $projectDir/src/*.d $projectDir/libuuid/src/*.o
  XALT_EPOCH_T0=$(python3 $projectDir/rt/xalt_epoch.py)

}

displayThis()
{
  XALT_EPOCH_T1=$(LD_PRELOAD= PATH=$outputDir/XALT/xalt/xalt/libexec:$outputDir/XALT/$VERSION/libexec:$PATH xalt_epoch "$XALT_EPOCH_T0")
  echo 
  echo "#==========================================================#"
  echo "  ""$@"
  echo "  ($XALT_EPOCH_T1)"
  echo "#==========================================================#"
}

buildRmapT()
{
  echo "<build xalt_rmapT.json file>"
  rm -rf reverseMapD
  mkdir  reverseMapD
  MPATH=${LMOD_DEFAULT_MODULEPATH-$MODULEPATH}
  $LMOD_DIR/spider -o xalt_rmapT      $MPATH > $outputDir/reverseMapD/xalt_rmapT.json
  $LMOD_DIR/spider -o jsonReverseMapT $MPATH > $outputDir/reverseMapD/jsonReverseMapT.json
  echo "<finish>"
}

installXALT()
{
  rm -rf XALT build .make_failed
  mkdir build

  (cd build; echo "<configure>";
   echo $projectDir/configure --prefix $outputDir/XALT --with-etcDir=$outputDir --with-config=$projectDir/Config/rtm_config.py "$@" ; 
        $projectDir/configure --prefix $outputDir/XALT --with-etcDir=$outputDir --with-config=$projectDir/Config/rtm_config.py "$@" ; 
   
   XALT_MAKE_PARALLEL=4
   if [ -n "${XALT_BUILD_DEBUG+x}" ]; then
     XALT_MAKE_PARALLEL=1
   fi

   echo "<make>"; make -j $XALT_MAKE_PARALLEL OPTLVL="-g -O0" install Inst_TACC Inst_RT;
   if  [ "$?" != 0 ]; then
       echo "make failed"
       touch $outputDir/.make_failed
   fi
  )
  if [ -f $outputDir/.make_failed ]; then
      rm $outputDir/.make_failed 
      exit 1;
  fi
  echo "<end make>"
}

installDB()
{
  echo "<create db>"
  DBNAME=testxalt
  DB_CONF_FN=testxalt_db.conf
  DBUSER=xaltTest
  PASSWD=KutWgbh1@

  echo PATH=$PATH

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
