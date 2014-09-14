#!/bin/bash
# -*- shell-script -*-

initialize()
{
  PATH=$outputDir/XALT/bin:$outputDir/XALT/sbin:$PATH;

  ORIG_HOME=`(cd $HOME; /bin/pwd)`
  HOME=`/bin/pwd`
  numStep=0
  COUNT=0

  rm -f  _stderr.* _stdout.* out.* err.* 
  rm -rf .xalt.d

}

installXALT()
{
  rm -rf XALT
  make -f $projectDir/makefile prefix=$outputDir/XALT PATH_TO_SRC=$projectDir \
    install > /dev/null
  cp $projectDir/src/removeDataBase.py    XALT/sbin
  cp $projectDir/test/check_entries_db.py XALT/sbin
}

installDB()
{
  DBNAME=testxalt
  DBUSER=xaltTest
  PASSWD=kutwgbh

  conf_create.py     --dbhost localhost --dbuser $DBUSER \
                     --passwd $PASSWD   --dbname $DBNAME
  removeDataBase.py  --dbname $DBNAME  > /dev/null 2>&1
  createDB.py        --dbname $DBNAME
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
