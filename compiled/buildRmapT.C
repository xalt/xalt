#include <stdlib.h>
#include "run_submission.h"
#include "xalt_config.h"
#include "xalt_quotestring.h"
#include "jsmn.h"

void processRmapT(const char* js, int& i, int ntokens, jsmntok_t*  tokens, table& rmapT)
{
  if (tokens[i].type != JSMN_OBJECT)
    {
      fprintf(stderr,"(1) Bad xalt_rmapT.json file\n");
      exit(1);
    }

  int iend = tokens[i].end;

  ++i;
  while (i < ntokens)
    {
      if (tokens[i].start > iend)
        {
          --i;
          return;
        }

      if (tokens[i].type != JSMN_STRING && tokens[i+1].type != JSMN_STRING)
        {
          fprintf(stderr,"(2) Bad xalt_rmapT.json file\n");
          exit(1);
        }
      std::string key(  js, tokens[i].start, tokens[i].end - tokens[i].start); ++i;
      std::string value(js, tokens[i].start, tokens[i].end - tokens[i].start); ++i;
  
      rmapT[key] = value;
    }
}

void processXlibmap(const char* js, int& i, int ntokens, jsmntok_t* tokens,
                    std::vector<std::string>& xlibmapA)
{
  if (tokens[i].type != JSMN_ARRAY)
    {
      fprintf(stderr,"(3) Bad xalt_rmapT.json file\n");
      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n", i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
      exit(1);
    }

  int iend = tokens[i].end;

  ++i;
  while (i < ntokens)
    {
      if (tokens[i].start > iend)
        {
          --i;
          return;
        }

      if (tokens[i].type != JSMN_STRING)
        {
          fprintf(stderr,"(4) Bad xalt_rmapT.json file\n");
          exit(1);
        }
      std::string value(js, tokens[i].start, tokens[i].end - tokens[i].start); ++i;
      xlibmapA.push_back(value);
    }
}

void buildRmapT(table& rmapT)
{
  static char *extA[]  = {".json", ".old.json"};
  std::string rmapFn;
  char * xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  char* start = p;
  while(1)
    {
      char * p = strchr(xalt_etc_dir,':');
      if (p)
        {
          rmapFn.assign(start, p - start);  //fixMe.
        }
    
}
