#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

from __future__ import print_function
import os, sys, re, ConfigParser, getpass, base64

class CreateConf(object):
  def __init__(self):
    self.__host   = None
    self.__user   = None
    self.__passwd = None
    self.__db     = None
    
  def __readFromUser(self):
    self.__host   = raw_input("Database host: ")
    self.__user   = raw_input("Database user: ")
    self.__passwd = getpass.getpass("Database pass: ")
    self.__db     = raw_input("Database name: ")
    
  def __writeConfig(self):
    config=ConfigParser.ConfigParser()
    config.add_section("MYSQL")
    config.set("MYSQL","HOST",self.__host)
    config.set("MYSQL","USER",self.__user)
    config.set("MYSQL","PASSWD",base64.b64encode(self.__passwd))
    config.set("MYSQL","DB",self.__db)

    fn = self.__db + "_db.conf"

    f = open(fn,"w")
    config.write(f)
    f.close()

  def create(self):
    self.__readFromUser()
    self.__writeConfig()
    


def main():
  createConf = CreateConf()
  createConf.create()


if ( __name__ == '__main__'): main()
