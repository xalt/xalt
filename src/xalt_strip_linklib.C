// /usr/bin/ld --sysroot=/ --build-id --eh-frame-hdr -m elf_x86_64 --hash-style=gnu --as-needed -dynamic-linker /lib64/ld-linux-x86-64.so.2 -z relro -o helloññ
// /usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu/crt1.o /usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu/crti.o
// /usr/lib/gcc/x86_64-linux-gnu/4.8/crtbegin.o -L/opt/apps/gcc-4_8/mpich/3.1.2/lib -L/usr/lib/gcc/x86_64-linux-gnu/4.8
// -L/usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/4.8/../../../../lib
// -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/x86_64-linux-gnu -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-linux-gnu/4.8/../../.. /tmp/ccb8pYsD.o
// -rpath /opt/apps/gcc-4_8/mpich/3.1.2/lib --enable-new-dtags -lmpi -lrt -lpthread -lgcc --as-needed -lgcc_s --no-as-needed -lc
// -lgcc --as-needed -lgcc_s --no-as-needed /usr/lib/gcc/x86_64-linux-gnu/4.8/crtend.o /usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu/crtn.o


#include <string.h>
#include <stdio.h>

#include "buildRmapT.h"

int main(int argc, char* argv[])
{
  std::string rmapD = ""; // If this is an empty string then XALT_ETC_DIR is used to specify location of rmapD
  Table       rmapT;
  Vstring     xlibmapA;
  buildRmapT(rmapD, rmapT, xlibmapA);

  Set reflibSet;
  Vstring resultA;
  std::string libname;
  std::string::size_type idx;

  for (auto const & it : xlibmapA)
    {
      idx = it.rfind(".a");
      if (idx != std::string::npos)
        libname = it.substr(0,idx);  // from trailing "." to end of string
      else
        {
          idx = it.rfind(".so");
          if (idx != std::string::npos)
            libname = it.substr(0,idx);  // from trailing "." to end of string
          else
            libname = it;
        }
      reflibSet.insert(libname);
    }

  bool o_flag = false;
  for (int i = 1; i < argc; ++i)
    {
      char *p = argv[i];

      // look for -lname => convert to libname
      if (*p == '-' && *(p+1) == 'l')
        {
          p += 2;
          libname.assign("lib");
          libname.append(p);
        }
      else if (*p == '-' && *(p+1) == 'o')
        {
          o_flag = true;
          continue;
        }
      else if (o_flag)
        {
          o_flag = false;
          continue;
        }
      else if (*p != '/')
        {
          resultA.push_back(argv[i]);
          continue;
        }
      else
        {
          // if here there is a leading '/'
          // find the basename of this path
          p = strrchr(p,'/');  
          ++p;

          // check for libname.so or libname.a
          char* e = strrchr(p,'.');
          if (e && ((*(e+1) == 'a') || (*(e+1) == 's'  && *(e+2) == 'o')))
            libname.append(p, e-p);
          else
            {
              resultA.push_back(argv[i]);
              continue;
            }
        }
            
      Set::const_iterator got = reflibSet.find(libname);
      if ( got == reflibSet.end() )
        resultA.push_back(argv[i]);
    }          

  for (auto const & it : resultA)
    fprintf(stdout," %s",it.c_str());
  fprintf(stdout,"\n");

  return 0;
}
