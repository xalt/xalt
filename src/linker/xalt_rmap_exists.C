#include <stdio.h>
#include "xalt_config.h"
#include "xalt_utils.h"

int main()
{
  // If this is an empty string then XALT_ETC_DIR is used to specify location of rmapD
  std::string rmapD = ""; 
  FILE *fp = xalt_json_file_open(rmapD, "reverseMapD/xalt_rmapT");
  if (fp == NULL)
    {
      fp = xalt_json_file_open(rmapD, "xalt_rmapT");
      if (fp == NULL)
        return 1;
    }
  return 0;
}
