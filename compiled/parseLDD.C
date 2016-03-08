#include <iostream>
#include "run_submission.h"
#include "xalt_config.h"

void parseLDD(std::string& exec, std::vector<Libpair>& libA)
{
  std::string              cmd;
  std::vector<std::string> result;

  // Capture the result from running ldd on the executable
  cmd  = LDD " " + exec + " 2> /dev/null";
  capture(cmd, result);

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

  std::string::size_type s1,s2;
  std::string            lib, sha1;
  int sz = result.size();
  for (int i = 0; i < sz; ++i)
    {
      std::vector<std::string> sha1_result;
      
      std::string& s = result[i];
      s1 = s.find("=> /");
      if (s1 == std::string::npos)
        continue;
      s2 = s.find(" (",s1);
      lib = s.substr(s1+3, s2-(s1+3)); 

      // Find sha1sum of the library just found.
      // The output of sha1sum looks like:
      //   % sha1sum /lib/x86_64-linux-gnu/libm.so.6
      //   3dfbedb82b999ae7bb14a873439810a0a7cf94a0  /lib/x86_64-linux-gnu/libm.so.6

      cmd = SHA1SUM " " + lib;
      capture(cmd, sha1_result);
      s1 = sha1_result[0].find(" ");
      sha1 = sha1_result[0].substr(0, s1);

      Libpair libpair(lib, sha1);
      libA.push_back(libpair);
    }
}
