#include "link_submission.h"

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
        libname = (*it).substr(idx);  // from trailing "." to end of string
      else
        {
          idx = (*it).rfind(".so");
          if (idx != std::string::npos)
            libname = (*it).substr(idx);  // from trailing "." to end of string
          else
            libname = *it;
        }
      reflibSet.insert(libname);
    }

  // 

  for (int i = 1; i < argc; ++i)
    {
      char *p = argv[i];
      if (*p == '-' && *(p+1) == 'l')
        {
          libname.assign("lib");
          char* e = strrchr(p,'.');
          if ((*(e+1) == 'a') || (*(e+1) == 's'  && *(e+2) == 'o'))
            libname.append(p, e-p);
          else
            libname.append(p);
        }
      else
        libname = p;

      
      Set::const_iterator got = reflibSet.find(libname);
      if ( got == reflibSet.end() )
        resultA.push_back(argv[i]);
    }

  for (auto it = resultA.begin(); it != result.end() ++it)
    printf(" %s",*it);
  printf("\n");
  

  return 0;
}



