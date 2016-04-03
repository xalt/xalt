#include <string.h>
#include "buildRmapT.h"
#include <stdio.h>

//FILE* xalt_log;

int main(int argc, char* argv[])
{
  Table   rmapT;
  Vstring xlibmapA;
  buildRmapT(rmapT, xlibmapA);

  Set reflibSet;
  Vstring resultA;
  std::string libname;
  std::string::size_type idx;

  for (auto it = xlibmapA.begin(); it != xlibmapA.end(); ++it)
    {
      idx = (*it).rfind(".a");
      if (idx != std::string::npos)
        libname = (*it).substr(0,idx);  // from trailing "." to end of string
      else
        {
          idx = (*it).rfind(".so");
          if (idx != std::string::npos)
            libname = (*it).substr(0,idx);  // from trailing "." to end of string
          else
            libname = *it;
        }
      reflibSet.insert(libname);
    }

  for (int i = 1; i < argc; ++i)
    {
      // Find ``basename'' for argv[i]
      char *p = strrchr(argv[i],'/');

      // reset p if there is no trailing '/' or bump past it.
      if (p == NULL)
        p = argv[i];
      else
        ++p;
      
      // look for -lname => convert to libname
      if (*p == '-' && *(p+1) == 'l')
        {
          p += 2;
          libname.assign("lib");
          libname.append(p);
        }
      else
        {

          // check for libname.so or libname.a
          char* e = strrchr(p,'.');
          if (e && ((*(e+1) == 'a') || (*(e+1) == 's'  && *(e+2) == 'o')))
            libname.append(p, e-p);
          else
            // take it.
            libname = p;
        }

      Set::const_iterator got = reflibSet.find(libname);
      if ( got == reflibSet.end() )
        resultA.push_back(argv[i]);
    }

  for (auto it = resultA.begin(); it != resultA.end(); ++it)
    fprintf(stdout," %s",(*it).c_str());
  fprintf(stdout,"\n");

  return 0;
}
