#!/usr/bin/env python
# -*- python -*-
from __future__ import print_function
import os, sys, socket, time, platform

def print_assembly(uuid, fn, version, epochStr):
  name    = socket.getfqdn()
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
  f.writelines("\t.asciz \"<Build.Machine>%%"+name+"%%\"\n")
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
  fn       = sys.argv[2]
  version  = "0.1"
  epochStr = str(time.time())
  
  print_assembly(uuid, fn, version, epochStr)

  print(epochStr)


if ( __name__ == '__main__'): main()
