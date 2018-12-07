#include "xalt_fgets_alloc.h"
#include "run_submission.h"
#include "build_xalt_tmpdir.h"
#include <dirent.h>
#include <fnmatch.h>
#include <stdio.h>

void transmitPkgRecords(Options& options)
{
  char * xalt_tmpdir = build_xalt_tmpdir(options.uuid().c_str());
  DIR*   dirp        = opendir(xalt_tmpdir);
  if (dirp == NULL)
    return;

  const char * transmission = getenv("XALT_TRANSMISSION_STYLE");
  if (transmission == NULL)
    transmission = TRANSMISSION;

  struct direct dp;
  while ( (dp = readdir(dirp)) != NULL)
    {
      if (fnmatch("pkg.*.json", dp->name, 0) == 0)
        {
          char*       buf     = NULL;
          size_t      sz      = 0;
          std::string jsonStr = "";
          std::string fullName(xalt_tmpdir);
          
          fullName.append("/");
          fullName.append(dp->d_name);
          FILE* fp = fopen(fullName.c_str(), "r");
          if (fp)
            {
              while( xalt_fgets_alloc(fp, &buf, &sz))
                jsonStr += buf;
              free(buf);
              sz = 0; buf = NULL;

              // transmit jsonStr
              transmit(transmission, jsonStr.c_str(), "pkg"
              

            }



  free(xalt_tmpdir);

}
