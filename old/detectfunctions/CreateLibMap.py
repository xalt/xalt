#!/usr/bin/python
#
# CreateLibMap.py
#   create a list of all libraries (and associated directories)
#   based on input from file <dir>
#
# Usage:
#    ListLibs.py <filewithdirs>
#
# Input:
#    dirs.xml (file) - list of include and exclude directories
#
#
import os;
import xml.etree.ElementTree as ET
import json;
from collections import defaultdict

class NestedDict(dict):
    def __getitem__(self, key):
        if key in self: return self.get(key)
        return self.setdefault(key, NestedDict())

#f = open('dirs', 'r')

tree = ET.ElementTree(file='dirs.xml')
root = tree.getroot()

# masked out (or in) subdirs in install directory 
maskinclude = []
#maskexclude = []

dirs = NestedDict() # will hold list of dirs to recursively go through

for dir in root:
   ddir = str(dir.attrib.values()).strip('[]').strip('\'')
   if ddir == "installmask":
      include = dir.find('include').find('list')
      maskinclude = include.text.split(' ')
   else:
      include = dir.find('include').find('list')
      exclude = dir.find('exclude').find('list')
      dirs[ddir]['include'] = include.text.split(' ')
      dirs[ddir]['exclude'] = exclude.text.split(' ')

libmap = defaultdict(list)
for path, v in dirs.iteritems():
   for kind, dirlist in v.iteritems():
      ddirs =  dirlist
      if kind == "include":
         # traverse everything in path
         if dirlist == ['*']:
            # special case to set ddirs variable
            ddirs = os.listdir(path)
            # remove any "top level" excludes if matched, but not any subdirs
            for pattern in dirs[path]['exclude']:
               if ddirs.index(pattern):
                   i = ddirs.index(pattern)
                   del ddirs[i]
                   # and since we excluded it from top level, dont iterate on it later
                   i = dirs[path]['exclude'].index(pattern)
                   del dirs[path]['exclude'][i]
#         print path, kind, ddirs


         infos = []
         for subdir in ddirs:
#            print path+'/'+subdir
            # Walk directory tree
            for ppath, ddir, files in os.walk(path+'/'+subdir, topdown=True):
               ddir[:] = [d for d in ddir if d not in dirs[path]['exclude']]
#               print ppath, ddir
               if ppath.split("/")[-1] in maskinclude:
                  if files:
                    for f in files:
                        if f.endswith(('.a', '.so')):
#                          print ppath, f
                          libmap[f].append(ppath)

j = json.dumps( libmap )
f = open('libmap.json','w')
print >> f, j
f.close()


