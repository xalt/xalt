#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, time, platform

def extract_compiler(pstree):
  ignoreT = {
    'pstree'   : True,
    'ld'       : True,
    'collect2' : True,
    }
    
  a = pstree.split("---")
  n = len(a)

  result = "unknown"
  for cmd in reversed(a):
    if (not (cmd in ignoreT)):
      result = cmd
      break

  return cmd

def print_assembly(uuid, fn, version, syshost, compiler, epochStr):
  user    = os.environ["USER"]
  osName  = platform.system() + "_%_%_" + platform.release()

  year  = time.strftime("%Y")
  date  = time.strftime("%c").replace(" ","_%_%_")

  f    = open(fn,"w")
  f.writelines("\t.section .xalt\n")
  f.writelines("\t.asciz \"XALT_Link_Info\"\n") #this is how to find the section in the exec
  # Print cushion
  f.writelines("\n\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n")
  f.writelines("\t.asciz \"<XALT_Version>%%"+version+"%%\"\n")
  f.writelines("\t.asciz \"<Build.Machine>%%"+syshost+"%%\"\n")
  f.writelines("\t.asciz \"<Build.compiler>%%"+compiler+"%%\"\n")
  f.writelines("\t.asciz \"<Build.OS>%%"+osName+"%%\"\n")
  f.writelines("\t.asciz \"<Build.User>%%"+user+"%%\"\n")
  f.writelines("\t.asciz \"<Build.UUID>%%"+uuid+"%%\"\n")
  f.writelines("\t.asciz \"<Build.Year>%%"+year+"%%\"\n")
  f.writelines("\t.asciz \"<Build.date>%%"+date+"%%\"\n")
  f.writelines("\t.asciz \"<Build.Epoch>%%"+epochStr+"%%\"\n")
  f.writelines("\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n")
  f.writelines("\t.asciz \"XALT_Link_Info_End\"\n")


def main():
  uuid     = sys.argv[1]
  syshost  = sys.argv[2]
  pstree   = sys.argv[3]
  fn       = sys.argv[4]
  version  = "0.1"
  epochStr = str(time.time())

  compiler = extract_compiler(pstree)

  print_assembly(uuid, fn, version, syshost, compiler, epochStr)

  print(epochStr)


if ( __name__ == '__main__'): main()
