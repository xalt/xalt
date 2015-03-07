# -*- python -*-
#
# Git Version: @git@

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
import os, sys, time, platform

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from xalt_util  import config_logger, extract_compiler

logger = config_logger()

def print_assembly(uuid, fn, version, syshost, compiler, epochStr):
  """
  Build the XALT assembly code

  @param uuid:     UUID string
  @param fn:       The output file name
  @param version:  Current XALT version
  @param syshost:  System name (darter, stampede), not login1.stampede.tacc.utexas.edu
  @param compiler: the name of the linking compiler
  @param epochStr: Current Timestamp
  """
  user    = os.environ.get("USER","unknown")
  osName  = platform.system() + "_%_%_" + platform.release()
  system  = platform.system()

  year  = time.strftime("%Y")
  date  = time.strftime("%c").replace(" ","_%_%_")

  try: 
    f    = open(fn,"w")

    if (system == "Darwin"): 
      f.writelines("\t.section .XALT, .xalt\n")
    else:
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
  """ Generate XALT assembly code """
  try: 
    uuid     = sys.argv[1]
    syshost  = sys.argv[2]
    fn       = sys.argv[3]
    version  = "@version@"
    epochStr = str(time.time())

    compiler = extract_compiler()

    print_assembly(uuid, fn, version, syshost, compiler, epochStr)

    print(epochStr)
  except:
    logger.exception("XALT_EXCEPTION:xalt_generate_assembly")


if ( __name__ == '__main__'): main()
