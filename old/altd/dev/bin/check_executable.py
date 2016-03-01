#! /usr/bin/python

import subprocess
import os
import sys

output = subprocess.Popen(["objdump","-s","-j",".altd",sys.argv[1]], stdout=subprocess.PIPE).communicate()[0]


print "ALTD Executable Checker"

output = output.split("\n")
output.pop(0)
output.pop(0)
output.pop(0)
output.pop(0)

altd = ""

for line in output:
    split = line.split()

    if len(split) > 0:

        altd = altd + split[-1]

fields = altd.split(":")


version = "This field could not be extracted from the source file"
machine = "This field could not be extracted from the source file"
tag_id = "This field could not be extracted from the source file"
year = "This field could not be extracted from the source file"

i = 0
for field in fields:
    
    if "Version" in field:
        version = fields[i+1]
    if "Machine" in field:
        machine = fields[i+1]
    if "Tag_id" in field:
        tag_id = fields[i+1]
    if "Year" in field:
        year = fields[i+1]
        

    i = i +1

print "Executable:" + os.path.abspath(sys.argv[1])
print "Version: " + version 
print "Machine: " + machine
print "Tag ID: " + tag_id
print "Year: " + year 


#os.system("objdump -s -j .altd " + sys.argv[1])



