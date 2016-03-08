#include "run_submission.h"
#include "xalt_config.h"

void extractXALTRecord(std::string& exec, std::unordered_map<std::string, std::string>& recordT)
{
  std::string n, v, cmd;
  std::vector<std::string> result;

  // Capture the XALT record (if it exists) into result.
  cmd  = OBJDUMP " -s -j .xalt ";
  cmd += exec;
  cmd += " 2> /dev/null";
    
  capture(cmd, result);

  int  i;
  int  nlines = result.size();
  bool found  = false;

  for (i = 0; i < nlines; ++i)
    {
      if (result[i].find("Contents of section") != std::string::npos)
        {
          found = true;
          break;
        }
    }

  // If "Contents of section" was not found then exec doesn't have an XALT section => return 
  if (! found)
    return;

  /*
   * The results of the objdump command looks like this (after the "Contents ..." line)
   *  0000 58414c54 5f4c696e 6b5f496e 666f0000  XALT_Link_Info..
   *  0010 00000000 00003c58 414c545f 56657273  ......<XALT_Vers
   *  0020 696f6e3e 2525302e 36252500 3c427569  ion>%%0.6%%.<Bui
   *  0030 6c642e53 7973686f 73743e25 25766d69  ld.Syshost>%%vmi
   *  0040 6a6f2525 003c4275 696c642e 636f6d70  jo%%.<Build.comp
   *  0050 696c6572 3e252567 63632525 003c4275  iler>%%gcc%%.<Bu
   *  0060 696c642e 636f6d70 696c6572 50617468  ild.compilerPath
   *  0070 3e25252f 7573722f 62696e2f 6763632d  >%%/usr/bin/gcc-
   *  0080 342e3825 25003c42 75696c64 2e4f533e  4.8%%.<Build.OS>
   *  0090 25254c69 6e75785f 255f255f 332e3133  %%Linux_%_%_3.13
   *  00a0 2e302d37 372d6765 6e657269 63252500  .0-77-generic%%.
   *  00b0 3c427569 6c642e55 7365723e 25256d63  <Build.User>%%mc
   *  00c0 6c617925 25003c42 75696c64 2e555549  lay%%.<Build.UUI
   *  00d0 443e2525 66643466 62393061 2d636133  D>%%fd4fb90a-ca3
   *  00e0 632d3462 32322d38 6666382d 65363261  c-4b22-8ff8-e62a
   *  00f0 65616535 39643439 2525003c 4275696c  eae59d49%%.<Buil
   *  0100 642e5965 61723e25 25323031 36252500  d.Year>%%2016%%.
   *  0110 3c427569 6c642e64 6174653e 25255475  <Build.date>%%Tu
   *  0120 655f255f 255f4665 625f255f 255f3233  e_%_%_Feb_%_%_23
   *  0130 5f255f25 5f30393a 35303a35 355f255f  _%_%_09:50:55_%_
   *  0140 255f3230 31362525 003c4275 696c642e  %_2016%%.<Build.
   *  0150 45706f63 683e2525 31343536 32343236  Epoch>%%14562426
   *  0160 35352e39 34252500 00000000 00000058  55.94%%........X
   *  0170 414c545f 4c696e6b 5f496e66 6f5f456e  ALT_Link_Info_En
   *  0180 6400                                 d.              
   */


  // Take the output above and convert the string portion into a single line

  std::string            xaltRecord;
  std::string::size_type idx, len;
  for (i = i + 1; i < nlines; i++)
    {
      std::string& line = result[i];
      idx               = line.find("  ");
      idx               = line.find_first_not_of(" ",idx);
      len               = line.size();
      xaltRecord       += line.substr(idx, len-idx-1);
    }

  // Extract key value pairs from xaltRecord.
  std::string::size_type s1, s2, s3, s4;

  s1 = 0;
  while(1)
    {
      s1 = xaltRecord.find("<", s1);
      if (s1 == std::string::npos)
        break;

      s1++;
      s2 = xaltRecord.find(">%%",s1);
      n  = xaltRecord.substr(s1,s2-s1);

      s3 = xaltRecord.find("%%.",s2);
      
      v = xaltRecord.substr(s2+3,s3 - (s2+3));

      s4 = 0;
      while(1)
        {
          s4 = v.find("_%_%_", s4);
          if (s4 == std::string::npos)
            break;
          v.replace(s4,5," ");
        }

      recordT[n] = v;
    }
}
      
      
  
  
  
  


