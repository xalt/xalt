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

import os
import sys
import json
from   xalt_global import *
from   collections import defaultdict

def convert(input):
    if isinstance(input, dict):
        return dict([(convert(key), convert(value)) for key, value in input.iteritems()])
    elif isinstance(input, list):
        return [convert(element) for element in input]
    elif isinstance(input, unicode):
        return input.encode('utf-8')
    else:
        return input

# Build the reference list of libraries to remove from link line
fm = os.path.join(XALT_ETC_DIR, "funcMapLib.json")
fp = open(fm)
j = convert(json.load(fp))

refLibs = set()
for lib in j.keys():
  name = lib.replace('.a','').replace('.so','')
  refLibs.add(name)


# Remove libraries that are in reference lib
linkline = sys.argv[1:]
xLibs    = []
for iLib in linkline:
  testLib = os.path.basename(iLib.replace("-l", "lib") \
                             .replace('.a','').replace('.so','').strip())
  if testLib in refLibs:
    xLibs.append(iLib)
    
for iLib in xLibs:
  linkline.remove(iLib)
  
print " ".join(linkline)
