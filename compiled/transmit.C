#include <iostream>
#include <sstream>
#include <fstream>
#include "transmit.h"
#include "base64.h"
#include "xalt_types.h"
#include "zstring.h"

void transmit(const char* transmission, std::string jsonStr, const char* kind, std::string key, const char* resultFn)
{
    if (strcasecmp(transmission, "file") == 0)
    {
      //*********************************************************************
      // Build file name for xalt json record.  It is only used when the
      // file transmission style is used;
      if (resultFn == NULL)
        return;        

      std::string dirname(resultFn);
      dirname.erase(dirname.rfind('/'), std::string::npos);
      if (! isDirectory(dirname.c_str()))
        mkdir (dirname.c_str(),0700);


      std::ofstream myfile;
      myfile.open(resultFn);
      myfile << jsonStr;
      myfile.close();
    }
  else if (strcasecmp(transmission, "syslogv1") == 0)
    {
      std::ostringstream cmd;
      std::string b64     = base64_encode(reinterpret_cast<const unsigned char*>(jsonStr.c_str()), jsonStr.size());
      cmd << LOGGER " -t XALT_LOGGING \"" << kind << ":" << syshost;
      cmd << ":" << b64 << "\"";
      system(cmd.str().c_str());
    }

  else if (strcasecmp(transmission, "syslog") == 0)
    {
      std::ostringstream cmd;
      std::string zs    = compress_string(json.result());
      std::string b64   = base64_encode(reinterpret_cast<const unsigned char*>(zs.c_str()), zs.size());
      int         sz    = b64.size();
      int         blkSz = (sz < syslog_msg_sz) ? sz : syslog_msg_sz;
      int         nBlks = (sz -  1)/blkSz + 1;
      int         istrt = 0;
      int         iend  = blkSz;

      for (int i = 0; i < nBlks; ++i)
        {
          cmd << LOGGER " -t XALT_LOGGING V:2 kind:" << key << " idx:" << i;
          cmd << " nb:"  << nBlks << " syshost:" << options.syshost();
          cmd << " key:" << key   << " value:"   << b64.substr(istrt, iend - istrt);
          system(cmd.str().c_str());
          istrt = iend;
          iend  = istrt + blkSz;
          if (iend > sz)
            iend = sz;
        }
    }
}
