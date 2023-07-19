#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, re, math

def isPrime(n):
  # Corner cases
  if (n==0 or n == 2 or n==3):
    return True
  elif (n <= 1):
    return False

  if (n % 2 == 0 or n % 3 == 0):
    return False

  i = 5
  while (i*i <= n) :
    if ( n%i == 0 or n % (i + 2) == 0):
      return False
    i = i + 6
  return True
            
def main():
  try:
    i = int(sys.argv[1])
  except:
    print("Prime number value must be a number not:", sys.argv[1])
    sys.exit(1)
  if (i < 0):
    sys.exit(-1)
  
  if (not isPrime(i)):
    sys.exit(-1)
    

if ( __name__ == '__main__'): main()
