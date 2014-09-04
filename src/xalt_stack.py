#!/usr/bin/env python
# -*- python -*-

from __future__ import print_function
import os, sys, re
class Stack(object):
  def __init__(self):
    self.__items = []
         
  #method for pushing an item on a stack
  def push(self,item):
    self.__items.append(item)
         
  #method for popping an item from a stack
  def pop(self):
    return self.__items.pop()
   
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
