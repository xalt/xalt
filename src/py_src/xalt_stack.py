#!/usr/bin/env python
# -*- python -*-

#-----------------------------------------------------------------------
# XALT: A tool that tracks users jobs and environments on a cluster.
# Copyright (C) 2013-2014 University of Texas at Austin
# Copyright (C) 2013-2014 University of Tennessee
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
  """ Basic stack class """
  def __init__(self):
    """
    initialize array that is used for the stack.
    """
    self.__items = []
         

  def push(self,item):
    """ method for pushing an item on a stack """
    self.__items.append(item)

  def pop(self):
    """ method for popping an item from a stack """
    v = self.__items.pop()
    return v
   
  def isEmpty(self):
    """ method to check whether the stack is empty or not """
    return (self.__items == [])
   
  def contents(self):
    """ return the contents of the stack as a single text block. """
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
