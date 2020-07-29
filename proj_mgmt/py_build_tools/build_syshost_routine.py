
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
from pprint import pprint
import os, sys, re, argparse, json

def xalt_syshost_main(sA):
  sA.append("#ifdef HAVE_MAIN")
  sA.append("int main()")
  sA.append("{")
  sA.append("  char* syshost = xalt_syshost();")
  sA.append("  printf(\"%s\\n\",syshost);")
  sA.append("  free(syshost);")
  sA.append("  return 0;")
  sA.append("}")
  sA.append("#endif")
  sA.append("")

def hardcode(name,output):
  sA = []
  sA.append("#include <stdio.h>")
  sA.append("#include <string.h>")
  sA.append("#include \"xalt_obfuscate.h\"")
  sA.append("char * xalt_syshost() {")
  sA.append("  return strdup(\"" + name + "\");")
  sA.append("}")
  xalt_syshost_main(sA)

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()



def add_hostname_routine(sA):
  sA.append("#include <stdio.h>")
  sA.append("#include <string.h>")
  sA.append("#include <unistd.h>")
  sA.append("#include <stdlib.h>")
  sA.append("#include <sys/utsname.h>")
  sA.append("#include <sys/types.h>")
  sA.append("#include <sys/socket.h>")
  sA.append("#include <netdb.h>")
  sA.append("#include <string.h>")
  sA.append("#include \"xalt_obfuscate.h\"")
  sA.append("#include \"xalt_c_utils.h\"")
  sA.append("")
  sA.append("char * hostname()")
  sA.append("{")
  sA.append("  char* my_hostname;")
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
  sA.append("      my_hostname = strdup(u.nodename);")
  sA.append("      return my_hostname;")
  sA.append("    }")
  sA.append("")
  sA.append("  my_hostname = strdup(info->ai_canonname);")
  sA.append("  freeaddrinfo(info);")
  sA.append("  return my_hostname;")
  sA.append("}")


def env_var(var, output):
  sA = []

  sA.append("#include <stdio.h>")
  sA.append("#include <stdlib.h>")
  sA.append("#include <string.h>")
  sA.append("#include \"xalt_obfuscate.h\"")
  sA.append("")
  sA.append("char * xalt_syshost()")
  sA.append("{")
  sA.append("  const char* var = getenv(\"" + var + "\");")
  sA.append("  if (!var) var = \"unknown\";")
  sA.append("  return strdup(var);")
  sA.append("}")
  xalt_syshost_main(sA)

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()

def read_file(fname,output):
  sA = []

  sA.append("#include <stdio.h>")
  sA.append("#include <stdlib.h>")
  sA.append("#include <string.h>")
  sA.append("#include \"xalt_obfuscate.h\"")
  sA.append("#include \"xalt_fgets_alloc.h\"")
  sA.append("")
  sA.append("char * xalt_syshost()")
  sA.append("{")
  sA.append("  const char* fname = \"" + fname + "\";")
  sA.append("  FILE *fp = fopen(fname,\"r\");")
  sA.append("  if (! fp)")
  sA.append("    fprintf(stderr,\"unable to open %s\\n\",fname);")
  sA.append("")
  sA.append("  size_t sz         = 0;")
  sA.append("  char * my_syshost = NULL;")
  sA.append("  xalt_fgets_alloc(fp, &my_syshost, &sz);")
  sA.append("  int len = strlen(my_syshost);")
  sA.append("  if (len > 0 && (my_syshost[len-1] == '\\n'))")
  sA.append("    my_syshost[len-1] = '\\0'; // overwrite trailing newline")
  sA.append("  fclose(fp);")
  sA.append("  return my_syshost;")
  sA.append("}")
  xalt_syshost_main(sA)

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

  sA.append("char* xalt_syshost()")
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
  sA.append("  char * w   = (char *) malloc(len+1);")
  sA.append("  memcpy(w,start,len);")
  sA.append("  w[len] = '\\0';")
  sA.append("  my_free(my_hostname, strlen(my_hostname));")
  sA.append("  return w;")
  sA.append("}")
  xalt_syshost_main(sA)

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()


def strip_nodename_numbers(output):
  sA = []

  add_hostname_routine(sA)
  sA.append("#include <string.h>")
  sA.append("#include <sys/utsname.h>")
  sA.append("#include \"xalt_obfuscate.h\"")
  sA.append("char* xalt_syshost()")
  sA.append("{")
  sA.append("  char * my_hostname = hostname();")
  sA.append("")
  sA.append("  int i;")
  sA.append("  int j = 0;")
  sA.append("")
  sA.append("  char * p = strchr(my_hostname,'.');")
  sA.append("  if (p)")
  sA.append("    *p = '\\0';")
  sA.append("")
  sA.append("  j = strcspn(my_hostname,\"0123456789\");")
  sA.append("")
  sA.append("  my_hostname[j] = '\\0';")
  sA.append("  return my_hostname;")
  sA.append("}")
  xalt_syshost_main(sA)

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()

def mapping(file,output):
  sA = []
  with open(file) as json_file:
    pairs = json.load(json_file)


  sA.append("#include <regex.h>")
  add_hostname_routine(sA)

  sA.append("struct Pair")
  sA.append("{")
  sA.append("  const char * key;")
  sA.append("  const char * syshost;")
  sA.append("};")
  sA.append("")

  sA.append("static struct Pair pair[] = {")
  for entry in pairs:
    sA.append("  {\"" + entry[0] + "\", \"" + entry[1] + "\"},")
  sA.append("};")


  sA.append("char * xalt_syshost()")
  sA.append("{")
  sA.append("  char        msgbuf[100];")
  sA.append("  regex_t     regex;")
  sA.append("  int         iret;")
  sA.append("  int         i;")
  sA.append("  int         sz = sizeof(pair)/sizeof(pair[0]);")
  sA.append("  const char* my_hostname = hostname();")
  sA.append("")
  sA.append("  for (i = 0; i < sz; ++i)")
  sA.append("    {")
  sA.append("      iret = regcomp(&regex, pair[i].key, 0);")
  sA.append("      if (iret)")
  sA.append("        {")
  sA.append("          fprintf(stderr,\"Could not compile regex: %s\\n\", pair[i].key);")
  sA.append("          exit(1);")
  sA.append("        }")
  sA.append("")
  sA.append("      iret = regexec(&regex, my_hostname, 0, NULL, 0);")
  sA.append("      if (iret == 0)")
  sA.append("        {")
  sA.append("           my_free(my_hostname, strlen(my_hostname));")
  sA.append("           return strdup(pair[i].syshost);")
  sA.append("        }")
  sA.append("      else if (iret != REG_NOMATCH)")
  sA.append("        {")
  sA.append("          regerror(iret, &regex, msgbuf, sizeof(msgbuf));")
  sA.append("          fprintf(stderr, \"pair[i].key Regex match failed: %s\\n\", msgbuf);")
  sA.append("          exit(1);")
  sA.append("        }")
  sA.append("      regfree(&regex);")
  sA.append("    }")
  sA.append("  return my_hostname;")
  sA.append("}")
  xalt_syshost_main(sA)

  s   = "\n".join(sA)
  f   = open(output,"w")
  f.write(s)
  f.close()


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
          ['read_file', read_file], ['mapping', mapping],
          ['strip_nodename_numbers', strip_nodename_numbers],
          ['env_var', env_var] ]

def main():
  args = CmdLineOptions().execute()

  # split argument at colon if there
  arg = args.input
  idx = arg.find(":")
  if (idx == -1):
    kind = arg.lower()
    opt  = kind == 'strip_nodename_numbers'
  else:
    kind = arg[:idx].lower()
    opt  = arg[idx+1:]

  # Check for valid argument and possible option, set [[func]] to be the function to build

  found = False
  for entry in kindA:
    if (kind == entry[0]):
      if (not opt):
        print("=================================================================")
        print("Unable to install XALT")
        print("Syshost config option \"%s\" does not have the required options" % kind)
        print("=================================================================")
        sys.exit(1)
      found = True
      func = entry[1]
      break

  if (not found):
    choices = ""
    for entry in kindA:
      choices += "\"" + entry[0] + "\", "
    print("=================================================================")
    print("Unable to install XALT")
    print("Syshost config \"%s\" is not valid, it must be one of %s" % (kind, choices))
    print("=================================================================")
    sys.exit(1)

  if (kind != 'strip_nodename_numbers'):
    # Build the user requested xalt_syshost.c routine
    func(opt, args.output)
  else:
    # strip_nodename doesn't need an option given to it
    func(args.output)

if ( __name__ == '__main__'): main()
