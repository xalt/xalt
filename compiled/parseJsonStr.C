#include <stdio.h>
#include <stdlib.h>

#include "parseJsonStr.h"
#include "xalt_fgets_alloc.h"
#include "jsmn.h"

void processArray(



void parseRunJsonStr(std::string& jsonStr, std::string& usr_cmdline, std::string& hash_id, Table& envT, Table& userT,
                     Table& recordT, std::vector<Libpair>& libA)
{
  
}




void parseLinkJsonStr(std::string& jsonStr, Vstring& linklineA, Table& resultT, std::vector<Libpair>& libA, Set& funcSet)
{
  jsmn_parser parser;
  jsmntok_t*  tokens;
  int         maxTokens = 1000;

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
      fprintf(stderr,"(5) Bad link.*.json file\n");
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
      if (mapName == "function")
        processFuncSet(js, i, ntokens, tokens, funcSet);
      else if (mapName == "linkA")
        processLibA(js, i, ntokens, tokens, libA);
      else if (mapName == "link_line")
        processArray(js, i, ntokens, tokens, linklineA);
      else if (mapName == "resultT")
        processTable(js, i, ntokens, tokens, resultT);
    }
}
