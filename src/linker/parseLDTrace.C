#include "xalt_config.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include "parseLDTrace.h"
#include "xalt_fgets_alloc.h"
#include "insert.h"

//void addPath2Set(std::string& path, Set& set)
//{
//  char* my_realpath = canonicalize_file_name(path.c_str());
//  if (my_realpath)
//    {
//      path.assign(my_realpath);
//      set.insert(path);
//      free(my_realpath);
//    }
//}

void addPath2Set(std::string& path, SET_t** libT)
{
  char* my_realpath = canonicalize_file_name(path.c_str());
  if (my_realpath)
    {
      insert_key_SET(libT, my_realpath);
      free(my_realpath);
    }
}

void parseLDTrace(const char* xaltobj, const char* linkfileFn, SET_t** libT)
{
  std::string path;
  char* buf = NULL;
  size_t sz = 0;
  
  FILE* fp = fopen(linkfileFn,"r");
  while(xalt_fgets_alloc(fp, &buf, &sz))
    {
      // Remove lines with ':'
      if (strchr(buf,':'))
        continue;

      // Remove the line with the xalt.o file
      if (strstr(buf, xaltobj))
        continue;

      // Remove any lines that start with /tmp/
      // This will ignore files like /tmp/ccT33qQt.o
      char * p = strstr(buf,"/tmp/");
      if (buf == p)
        continue;
      
      // Ignore all *.o files
      int len = strlen(buf);
      if (len > 2 && strstr(&buf[len-3],".o\n"))
        continue;

      char * start = strchr(buf,'(');
      if (start)
        {
          // Capture the library name in the parens:
          //    -lgcc_s (/usr/lib/gcc/x86_64-linux-gnu/4.8/libgcc_s.so)
          // or 
          //    (/usr/lib/x86_64-linux-gnu/libc_nonshared.a)elf-init.oS
          //
          // Note that we are going to ignore
          //    /usr/lib/x86_64-linux-gnu/libc_nonshared.a(elf-init.oS)

          char * p     = strchr(start+1,')');
          if ( start == NULL || p == NULL)
            continue;

          path.assign(start+1, p);
          addPath2Set(path, libT);
          continue;
        }

      // Save everything else (and get rid of the trailing newline!)
      path.assign(buf, len - 1);
      addPath2Set(path, libT);
    }
}

void readFunctionList(const char* fn, SET_t** funcSet)
{
  char*  buf = NULL;
  size_t sz  = 0;
  
  FILE* fp = fopen(fn,"r");

  // /tmp/ccCZTucS.o: In function `main':
  // /home/mclay/w/xalt/rt/mpi_hello_world.c:10: undefined reference to `MPI_Comm_rank'
  // /home/mclay/w/xalt/rt/mpi_hello_world.c:11: undefined reference to `MPI_Comm_size'
  // /home/mclay/w/xalt/rt/mpi_hello_world.c:16: undefined reference to `MPI_Finalize'


  const char * needle     = "undefined reference to ";
  int          len_needle = strlen(needle);

  while(xalt_fgets_alloc(fp, &buf, &sz))
    {
      // skip all lines that do not have "undefined references to "
      char* start = strstr(buf,needle);
      if (start == NULL)
        continue;

      start += len_needle;

      if (*start == '`')
        start++;
      char*  p   = strchr(start,'\'');
      if (p)
        *p = '\0';
      insert_key_SET(funcSet, start);
    }
  free(buf); sz  = 0; buf = NULL;
}
