#!/usr/bin/python
#
# StripLibs.py
#   strips out libraries from a link line given a reference list of 
#   libraries in the libmap.json file (produced by CreateLibMap.py)
#
# Usage:
#    StripLibs.py 
#
# Input:
#    ARGV environment variable must be the linkline arguments (from ld wrapper)
#
#
import os;
import json;
from collections import defaultdict

def convert(input):
    if isinstance(input, dict):
        return dict([(convert(key), convert(value)) for key, value in input.iteritems()])
    elif isinstance(input, list):
        return [convert(element) for element in input]
    elif isinstance(input, unicode):
        return input.encode('utf-8')
    else:
        return input

# get reference list of libraries to remove from link line
json_data=open('/nics/d/home/faheymr/Python/libmap.json')
j = convert(json.load( json_data ))
liblist = []
for lib in j.keys():
   name = lib.replace('.a','').replace('.so','')
   if name.startswith('lib'): name = name.replace('lib','')
   liblist.append(name)

# get link line
argv = os.getenv('ARGV')
argvsplit = argv.split(' ')
#print len(argvsplit)

# find matches in the link line based on reference list of libs 
indices = []
for arg in argvsplit:
   argstrip = arg.replace('-l',' ').strip(' ')
#   print arg, argstrip
   if argstrip in liblist:
#      print "found library match", arg, argstrip
      indices.append(arg)
#print indices

# need to remove found matches from the original link line
for index in indices:
   argvsplit.remove(index)
#print len(argvsplit)

argv = ' '.join(argvsplit)

# provide new link line back
print argv




