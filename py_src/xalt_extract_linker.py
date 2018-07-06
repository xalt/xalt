#-----------------------------------------------------------------------
# XALT: A tool that tracks users jobs and environments on a cluster.
# Copyright (C) 2013-2015 University of Texas at Austin
# Copyright (C) 2013-2015 University of Tennessee
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
#
# Git Version: @git@

from __future__                import print_function
import os, sys, json

dirNm, execName = os.path.split(os.path.realpath(sys.argv[0]))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../libexec")))
sys.path.insert(1,os.path.realpath(os.path.join(dirNm, "../site")))

from xalt_util import extract_compiler


def main():
  compiler, full_path_cmplr, link_line = extract_compiler()
  resultT = { 'compiler'  : compiler,
              'full_path' : full_path_cmplr,
              'link_line' : link_line
            }
  jsonStr = json.dumps(resultT)
  print(jsonStr)

  

if ( __name__ == '__main__'): main()
