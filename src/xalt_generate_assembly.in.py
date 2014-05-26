#!/usr/bin/env python
# -*- python -*-
#
# Git Version: @git@

from __future__ import print_function
from util       import config_logger, extract_compiler
import os, sys, time, platform

logger = config_logger()

def print_assembly(uuid, fn, version, syshost, compiler, epochStr):
  user    = os.environ.get("USER","unknown")
  osName  = platform.system() + "_%_%_" + platform.release()

  year  = time.strftime("%Y")
  date  = time.strftime("%c").replace(" ","_%_%_")

  try: 
    f    = open(fn,"w")
    f.writelines("\t.section .xalt\n")
    f.writelines("\t.asciz \"XALT_Link_Info\"\n") #this is how to find the section in the exec
    # Print cushion
    f.writelines("\n\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n")
    f.writelines("\t.asciz \"<XALT_Version>%%"+version+"%%\"\n")
    f.writelines("\t.asciz \"<Build.Syshost>%%"+syshost+"%%\"\n")
    f.writelines("\t.asciz \"<Build.compiler>%%"+compiler+"%%\"\n")
    f.writelines("\t.asciz \"<Build.OS>%%"+osName+"%%\"\n")
    f.writelines("\t.asciz \"<Build.User>%%"+user+"%%\"\n")
    f.writelines("\t.asciz \"<Build.UUID>%%"+uuid+"%%\"\n")
    f.writelines("\t.asciz \"<Build.Year>%%"+year+"%%\"\n")
    f.writelines("\t.asciz \"<Build.date>%%"+date+"%%\"\n")
    f.writelines("\t.asciz \"<Build.Epoch>%%"+epochStr+"%%\"\n")
    f.writelines("\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n")
    f.writelines("\t.asciz \"XALT_Link_Info_End\"\n")
  except:
    logger.exception("XALT_EXCEPTION:print_assembly")
    

def main():
  try: 
    uuid     = sys.argv[1]
    syshost  = sys.argv[2]
    pstree   = sys.argv[3]
    fn       = sys.argv[4]
    version  = "@version@"
    epochStr = str(time.time())

    compiler = extract_compiler(pstree)

    print_assembly(uuid, fn, version, syshost, compiler, epochStr)

    print(epochStr)
  except:
    logger.exception("XALT_EXCEPTION:xalt_generate_assembly")


if ( __name__ == '__main__'): main()
