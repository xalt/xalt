#!/usr/bin/env python
# -*- python -*-

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
import os, sys, re
class Stack(object):
  def __init__(self):
    self.__items = []
         
  #method for pushing an item on a stack
  def push(self,item):
    #print("Pushing: ", item)
    self.__items.append(item)

  #method for popping an item from a stack
  def pop(self):
    v = self.__items.pop()
    #print("Popping: ",v)
    return v
   
  #method to check whether the stack is empty or not
  def isEmpty(self):
    return (self.__items == [])
   
  def contents(self):
    return "\n".join(self.__items)


pstack = Stack()
def main():
  pstack.push("A")
  pstack.push("B")

  pstack.push("C1")
  pstack.pop()

  pstack.push("C2")
  pstack.push("D2")
  pstack.pop()
  pstack.pop()

  print(pstack.contents())



if ( __name__ == '__main__'): main()
