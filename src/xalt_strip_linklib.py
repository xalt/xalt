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

import os
import sys
import json
from   xalt_global import *
from   Rmap_XALT   import Rmap
from   collections import defaultdict

RMapFn = os.path.join(XALT_ETC_DIR,"reverseMapD")
libmap = Rmap(RMapFn).libMap()

refLibs = set()
for lib in libmap:
  name = lib.replace('.a','').replace('.so','')
  refLibs.add(name)

# Remove libraries that are in reference lib
workdir  = sys.argv[1]
linkline = sys.argv[2:]
aLibs    = []
for iLib in linkline:
  testLib = os.path.basename(iLib.replace("-l", "lib") \
                             .replace('.a','').replace('.so','').strip())
  if testLib in refLibs:
    continue
    
  # Replace /tmp/*.o object with our copy in WRKDIR
  if (iLib[:4] == '/tmp'):
    iLib = os.path.join("%s/obj/"%workdir, os.path.basename(iLib))
  
  aLibs.append(iLib)
    
print " ".join(aLibs)
