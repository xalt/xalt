#include <stdlib.h>
#include <string.h>
#include "compute_sha1.h"
#include "run_submission.h"
#include "capture.h"
#include "xalt_config.h"
#include "epoch.h"


ArgV            argV;

void parseLDD(std::string& exec, std::vector<Libpair>& libA, double& t_ldd, double& t_sha1)
{
  std::string  cmd;
  Vstring      result;

  // Capture the result from running ldd on the executable
  cmd  = LDD " " + exec + " 2> /dev/null";
  double t1 = epoch();
  capture(cmd, result);
  t_ldd = epoch() - t1;

  /*
   * The result of ldd look something like this:
   *    linux-vdso.so.1 =>  (0x00007ffcca6c1000)
   *    libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00002baa392c8000)
   *    libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00002baa395cc000)
   *    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00002baa397e2000)
   *    libuuid.so.1 => /lib/x86_64-linux-gnu/libuuid.so.1 (0x00002baa39ba7000)
   *    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00002baa39dac000)
   *    /lib64/ld-linux-x86-64.so.2 (0x00002baa390a3000)
   *
   *  We are only keeping the lines that match the pattern:
   *    libuuid.so.1 => /lib/x86_64-linux-gnu/libuuid.so.1 (0x00002baa39ba7000)
   */

  t1 = epoch();
  std::string::size_type s1,s2;
  std::string            lib, sha1;
    
  int sz = result.size();
  for (int i = 0; i < sz; ++i)
    {
      std::string& s = result[i];
      s1 = s.find("=> /");
      if (s1 == std::string::npos)
        continue;
      s2  = s.find(" (",s1);
      lib = s.substr(s1+3, s2-(s1+3));
      argV.push_back(Arg(lib));
    }
  long fnSzG = argV.size();

  compute_sha1_master(fnSzG);  // compute sha1sum for all files.

  for (long i = 0; i < fnSzG; ++i)
    {
      Libpair libpair(argV[i].fn, argV[i].sha1);
      libA.push_back(libpair);
    }
  t_sha1 = epoch() - t1;
}
