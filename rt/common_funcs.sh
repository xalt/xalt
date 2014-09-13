#!/bin/bash
# -*- shell-script -*-

initStdEnvVars()
{
  PATH=$outputDir/xalt/bin:$outputDir/xalt/sbin:$PATH;

  ORIG_HOME=`(cd $HOME; /bin/pwd)`
  HOME=`/bin/pwd`
  numStep=0
  COUNT=0
}

installXALT()
{
  rm -rf xalt
  make -f $projectDir/makefile prefix=$outputDir/xalt PATH_TO_SRC=$projectDir \
    install > /dev/null
  cp $projectDir/src/removeDataBase.py xalt/sbin
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

