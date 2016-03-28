#include "xalt_util.h"

FILE* xalt_file_open(const char* name)
{
  static char *extA[]  = {".json", ".old.json"};
  static int   nExt    = sizeof(extA)/sizeof(extA[0]);
  std::string fn;
  char * xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  FILE *fp = NULL;
  char* start = xalt_etc_dir;
  bool done = false;
  while(!done)
    {
      char * p = strchr(start,':');
      if (p)
        fn.assign(start, p - start);  
      else
        {
          fn.assign(start);
          done = true;
        }

      for (int i = 0; i < nExt; ++i)
        {
          fn += "/";
          fn += name;
          fn += extA[i];
          fp  = fopen(fn.c_str(), "r");
          if (fp)
            break;
        }
      start = ++p;
    } 
  return fp;
}
