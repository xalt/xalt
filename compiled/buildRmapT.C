#include <stdlib.h>
#include "run_submission.h"
#include "xalt_config.h"
#include "xalt_quotestring.h"
#include "jsmn.h"
#include "fget_alloc.h"

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
      key   = json_unquotestring(key);
      value = json_unquotestring(value);
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
      value = json_unquotestring(value);
      xlibmapA.push_back(value);
    }
}

void buildRmapT(table& rmapT, std::vector<std::string> xlibmap)
{
  static char *extA[]  = {".json", ".old.json"};
  static int   nExt    = sizeof(extA)/sizeof(extA[0]);
  std::string rmapFn;
  char * xalt_etc_dir = getenv("XALT_ETC_DIR");
  if (xalt_etc_dir == NULL)
    xalt_etc_dir = XALT_ETC_DIR;

  FILE *fp = NULL;
  char* start = xalt_etc_dir;
  while(1)
    {
      char * p = strchr(start,':');
      if (p)
        rmapFn.assign(start, p - start);  
      else
        rmapFn.assign(start);

      for (int i = 0; i < nExt; ++i)
        {
          rmapFn += "/xalt_rmapT";
          rmapFn += extA[i];
          fp      = fopen(rmapFn.c_str(), "r");
          if (fp)
            break;
        }
      start = ++p;
    } 
         
  if (fp == NULL)
    return;
  
  char* buf;

  std::string jsonStr = "";

  while((buf = fgets_alloc(fp)) != NULL)
    jsonStr += buf;

  jsmn_parser parser;
  jsmntok_t*  tokens;
  int maxTokens = 1000;

  tokens = (jsmntok_t *) malloc(sizeof(jsmntok_t)*maxTokens);

  jsmn_init(&parser);

  // js - pointer to JSON string
  // tokens - an array of tokens available
  int ntokens;

  const char * js = jsonStr.c_str();

  while (1)
    {
      ntokens = jsmn_parse(&parser, js, jsonStr.size(), tokens, maxTokens);
      if (ntokens > 0)
        break;
      if (ntokens == JSMN_ERROR_NOMEM)
        {
          maxTokens *= 2;
          tokens = (jsmntok_t *) realloc(tokens,sizeof(jsmntok_t)*maxTokens);
        }
    }      


  if (tokens[0].type != JSMN_OBJECT)
    {
      fprintf(stderr,"Bad xalt_rmapT.json file\n");
      exit(1);
    }

  int i = 1;
  while (i < ntokens)
    {
      if (tokens[i].type != JSMN_STRING )
        {
          fprintf(stderr,"Bad xalt_rmapT.json file\n");
          exit(1);
        }
      std::string mapName(js, tokens[i].start, tokens[i].end - tokens[i].start); ++i;
      if (mapName == "reverseMapT")
        processRmapT(js, i, ntokens, tokens, rmapT);
      else if (mapName == "xlibmap")
        processXlibmap(js, i, ntokens, tokens, xlibmapA);
      
    }
}
