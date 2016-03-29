#include <stdlib.h>
#include "xalt_config.h"
#include "buildRmapT.h"
#include "xalt_quotestring.h"
#include "jsmn.h"
#include "xalt_fgets_alloc.h"
#include "xalt_utils.h"

void processRmapT(const char* js, int& i, int ntokens, jsmntok_t*  tokens, Table& rmapT)
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
      key   = xalt_unquotestring(key.c_str());
      value = xalt_unquotestring(value.c_str());
      rmapT[key] = value;
    }
}

void processXlibmap(const char* js, int& i, int ntokens, jsmntok_t* tokens, Vstring& xlibmapA)
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
      value = xalt_unquotestring(value.c_str());
      xlibmapA.push_back(value);
    }
}

void buildRmapT(Table& rmapT, Vstring xlibmapA)
{
  
  FILE *fp = xalt_file_open("xalt_rmapT");
  if (fp == NULL)
    return;
  
  std::string jsonStr = "";

  char*  buf = NULL;
  size_t sz  = 0;

  while(xalt_fgets_alloc(fp, &buf, &sz))
    jsonStr += buf;
  free(buf);

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
