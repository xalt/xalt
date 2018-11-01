#!/usr/bin/python
#
# ParseFuncList.py
#
# Usage:
#    ParseFuncList.py 
#
# Input:
#    FUNCLIST file with list of unresolved functions -- output from ld
#
#
import os;
import re;
from collections import defaultdict

file=open('FUNCLIST')

rows = file.readlines()

# funcdict is a dictionary with function name as key and calling routine 
# list as values
funcdict = defaultdict(list)
for line in rows:
#    print line.strip()
    m = re.search('undefined reference to (.*)', line)
    if m:
       func = m.group(1).strip("`'")
#       print line.split(':')[0], func
#       print func
       funcdict[func].append(line.split(':')[0])

print funcdict
print funcdict.keys()
