
# -*- python -*-
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
import os, sys, re, argparse, json

def hardcode(name,output):
  sA = []
  sA.append("const char * xalt_syshost() {")
  sA.append("  return \"" + name + "\";")
  sA.append("}")

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()

def add_hostname_routine(sA):
  sA.append("#include <stdio.h>")
  sA.append("#include <unistd.h>")
  sA.append("#include <stdlib.h>")
  sA.append("#include <sys/utsname.h>")
  sA.append("#include <sys/types.h>")
  sA.append("#include <sys/socket.h>")
  sA.append("#include <netdb.h>")
  sA.append("#include <string.h>")
  sA.append("")
  sA.append("char * hostname()")
  sA.append("{")
  sA.append("  struct addrinfo hints, *info;")
  sA.append("  int gai_result;")
  sA.append("  struct utsname u;")
  sA.append("")
  sA.append("  uname(&u);")
  sA.append("  memset(&hints, 0, sizeof hints);")
  sA.append("  hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/")
  sA.append("  hints.ai_socktype = SOCK_STREAM;")
  sA.append("  hints.ai_flags = AI_CANONNAME;")
  sA.append("  ")
  sA.append("  if ((gai_result = getaddrinfo(u.nodename, \"http\", &hints, &info)) != 0) ")
  sA.append("    {")
  sA.append("      fprintf(stderr, \"getaddrinfo: %s\\n\", gai_strerror(gai_result));")
  sA.append("      exit(1);")
  sA.append("    }")
  sA.append("")
  sA.append("  char* my_hostname = strdup(info->ai_canonname);")
  sA.append("  freeaddrinfo(info);")
  sA.append("  return my_hostname;")
  sA.append("}")


def read_file(fname,output):
  sA = []

  sA.append("#include <stdio.h>")
  sA.append("#include <stdlib.h>")
  sA.append("#include <string.h>")
  sA.append("#define SZ 128")
  sA.append("")
  sA.append("char* fgets_alloc(FILE *fp)")
  sA.append("{")
  sA.append("  char* buf = NULL;")
  sA.append("  size_t size = 0;")
  sA.append("  size_t used = 0;")
  sA.append("  do {")
  sA.append("    size += SZ;")
  sA.append("    char *buf_new = realloc(buf, size);")
  sA.append("    if (buf_new == NULL) {")
  sA.append("      // Out-of-memory")
  sA.append("      free(buf);")
  sA.append("      return NULL;")
  sA.append("    }")
  sA.append("    buf = buf_new;")
  sA.append("    if (fgets(&buf[used], (int) (size - used), fp) == NULL) {")
  sA.append("      // feof or ferror")
  sA.append("      if (used == 0 || ferror(fp)) {")
  sA.append("        free(buf);")
  sA.append("        buf = NULL;")
  sA.append("      }")
  sA.append("      return buf;")
  sA.append("    }")
  sA.append("    size_t length = strlen(&buf[used]);")
  sA.append("    if (length + 1 != size - used) break;")
  sA.append("    used += length;")
  sA.append("  } while (buf[used - 1] != '\\n');")
  sA.append("  return buf;")
  sA.append("}")
  sA.append("")
  sA.append("const char * xalt_syshost()")
  sA.append("{")
  sA.append("  const char* fname = \"" + fname + "\";")
  sA.append("  FILE *fp = fopen(fname,\"r\");")
  sA.append("  if (! fp)")
  sA.append("    fprintf(stderr,\"unable to open %s\\n\",fname);")
  sA.append("")
  sA.append("  char * my_syshost = fgets_alloc(fp);")
  sA.append("  int len = strlen(my_syshost);")
  sA.append("  my_syshost[len-1] = '\\0'; // overwrite trailing newline")
  sA.append("  return my_syshost;")
  sA.append("}")

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()

  
def nth_name(nth,output):
  nth = int(nth)
  if (nth < 1 ):
    nth = 1

  sA = []
  add_hostname_routine(sA)

  sA.append("const char* xalt_syshost()")
  sA.append("{")
  sA.append("  char * my_hostname = hostname();")
  sA.append("")
  sA.append("  int nth      = " + str(nth) + ";")
  sA.append("  int i        = 0;")
  sA.append("  char * p     = my_hostname;")
  sA.append("  char * start = p;")
  sA.append("")
  sA.append("  while(1)")
  sA.append("    {")
  sA.append("      // Check for next dot")
  sA.append("      p = strchr(start,'.');")
  sA.append("      if (p == NULL)")
  sA.append("        return start; // There is no next dot")
  sA.append("      i++;")
  sA.append("      if (i >= nth)")
  sA.append("        break;")
  sA.append("      start = ++p;")
  sA.append("    }")
  sA.append("")
  sA.append("  int    len = p - start;")
  sA.append("  char * w   = malloc(len+1);")
  sA.append("  memcpy(w,start,len);")
  sA.append("  w[len] = '\\0';")
  sA.append("  return w;")
  sA.append("}")

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()


def mapping(file,output):
  sA = []
  add_hostname_routine(sA)

  print("mapping")
  

class CmdLineOptions(object):
  """ Command line Options class """

  def __init__(self):
    """ Empty Ctor """
    pass
  
  def execute(self):
    """ Specify command line arguments and parse the command line"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--input",    dest='input',    action="store", default="unknown",        help="input string")
    parser.add_argument("--output",   dest='output',   action="store", default="xalt_syshost.c", help="output c routine")
    args = parser.parse_args()
    
    return args

kindA = [ ['hardcode', hardcode ], ['nth_name', nth_name],
          ['read_file', read_file], ['mapping', mapping] ]

def main():
  args = CmdLineOptions().execute()

  # split argument at colon if there
  arg = args.input
  idx = arg.find(":")
  if (idx == -1):
    kind = arg.lower()
    opt  = None
  else:
    kind = arg[:idx].lower()
    opt  = arg[idx+1:]

  # Check for valid argument and possible option, set [[func]] to be the function to build

  found = False
  for entry in kindA:
    if (kind == entry[0]):
      if (not opt):
        print("Syshost config option \"%s\" does not have the required options" % kind)
        sys.exit(1)
      found = True
      func = entry[1]
      break

  if (not found):
    choices = ""
    for entry in kindA:
      choices += "\"" + entry[0] + "\", "
    print("Syshost config \"%s\" is not valid, it must be one of %s" % (kind, choices))
    sys.exit(1)


  # 

  func(opt, args.output)

if ( __name__ == '__main__'): main()
