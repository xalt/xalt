#include <stdio.h>
#include <stdlib.h>

#include "epoch.h"
#include "xalt_config.h"
#include "buildRmapT.h"
#include "xalt_quotestring.h"
#include "jsmn.h"
#include "xalt_fgets_alloc.h"
#include "xalt_utils.h"
#include "parseJsonStr.h"
#include "xalt_cxx_types.h"

void buildRmapT(std::string& rmapD, S2S_t** rmapT, UT_array** xlibmapA)
{
  
  FILE *fp = xalt_json_file_open(rmapD, "reverseMapD/xalt_rmapT");
  if (fp == NULL)
    {
      fp = xalt_json_file_open(rmapD, "xalt_rmapT");
      if (fp == NULL)
        return;
    }
  
  std::string jsonStr = "";

  char*  buf = NULL;
  size_t sz  = 0;

  while(xalt_fgets_alloc(fp, &buf, &sz))
    jsonStr += buf;
  free(buf);
  sz = 0; buf = NULL;

  jsmn_parser parser;
  jsmntok_t*  tokens;
  int maxTokens = 1000;

  tokens = (jsmntok_t *) malloc(sizeof(jsmntok_t)*maxTokens);

  jsmn_init(&parser);

  // js - pointer to JSON string
  // tokens - an array of tokens available
  int ntokens;

  if (jsonStr.size() == 0)
    jsonStr = "{}";
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
      fprintf(stderr,"(5) Bad xalt_rmapT.json file\n");
      exit(1);
    }


  int i = 1;
  while (i < ntokens)
    {
      if (tokens[i].type != JSMN_STRING )
        {
          fprintf(stderr,"(6) Bad xalt_rmapT.json file: i: %d, Type: %d\n", i, tokens[i].type);
          exit(1);
        }
      std::string mapName(js, tokens[i].start, tokens[i].end - tokens[i].start); ++i;
      if (mapName == "reverseMapT")
        processTable("xalt_rmapT.json",js, i, ntokens, tokens, rmapT);
      else if (mapName == "xlibmap")
        processArray("xalt_rmapT.json",js, i, ntokens, tokens, xlibmapA);
    }
  free(tokens);
}
