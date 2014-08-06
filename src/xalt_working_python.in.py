#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

def main():
  try:
    from __future__  import print_function
  except:
    return 1
  return 0


if ( __name__ == '__main__'):
  iret = main()
  sys.exit(iret)
