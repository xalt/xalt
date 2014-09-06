#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

#-----------------------------------------------------------------------
# XALT: A tool to track the programs on a cluster.
# Copyright (C) 2013-2014 Robert McLay and Mark Fahey
# 
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of 
# the License, or (at your option) any later version. 
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser  General Public License for more details. 
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA
#-----------------------------------------------------------------------

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
