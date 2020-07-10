#include <stdio.h>
#include <stdlib.h>

#include "xalt_quotestring.h"
#include "parseJsonStr.h"
void processArray(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, Vstring& vA)
{
  if (tokens[i].type != JSMN_ARRAY)
    {
      fprintf(stderr,"processArray for: %s, token type is not an array\n",name);
      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n",
              i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
      exit(1);
    }

  int iend = tokens[i].end;

  std::string  value;
  const char * p; 

  ++i;
  while (i < ntokens)
    {
      if (tokens[i].start > iend)
        return;

      if (tokens[i].type != JSMN_STRING)
        {
          fprintf(stderr,"processArray for: %s, token type is not a string\n",name);
          exit(1);
        }
      p = xalt_unquotestring(&js[tokens[i].start],tokens[i].end - tokens[i].start); ++i;
      value.assign(p);
      vA.push_back(value);
    }
}

void processValue(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, std::string& value)
{
  if (tokens[i].type != JSMN_STRING)
    {
      fprintf(stderr,"processValue for: %s, token type is not an object\n",name);
      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n", i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
      exit(1);
    }

  value.assign( js, tokens[i].start, tokens[i].end - tokens[i].start);
  i++;
}


void parseRunJsonStr(const char* name, std::string& jsonStr, std::string& usr_cmdline, std::string& hash_id,
                     Table& envT, Table& userT, DTable& userDT, Table& recordT, std::vector<Libpair>& libA,
                     std::vector<ProcessTree>& ptA)
{
  Table       measureT;
  jsmn_parser parser;
  jsmntok_t*  tokens;
  int         maxTokens = 1000;

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
      fprintf(stderr,"(5) Bad run.*.json file: %s\n",name);
      exit(1);
    }

  int i = 1;
  std::string mapName;
  while (i < ntokens)
    {
      if (tokens[i].type != JSMN_STRING )
        {
          fprintf(stderr,"(6) Bad file(%s): i: %d, Type: %d\n", name, i, tokens[i].type);
          fprintf(stderr,"%.*s\n",tokens[i].end - tokens[i].start, &js[tokens[i].start]);
          exit(1);
        }
      mapName.assign(&js[tokens[i].start], tokens[i].end - tokens[i].start); ++i;
      if (mapName == "cmdlineA")
        processA2JsonStr(name, js, i, ntokens, tokens, usr_cmdline);
      else if (mapName == "envT")
        processTable(name,js, i, ntokens, tokens, envT);
      else if (mapName == "hash_id")
        processValue(name,js, i, ntokens, tokens, hash_id);
      else if (mapName == "libA")
        processLibA(name, js, i, ntokens, tokens, libA);
      else if (mapName == "userT")
        processTable(name,js, i, ntokens, tokens, userT);
      else if (mapName == "userDT")
        processDTable(name,js, i, ntokens, tokens, userDT);
      else if (mapName == "ptA")
        processProcessTreeA(name,js, i, ntokens, tokens, ptA);
      else if (mapName == "xaltLinkT")
        processTable(name,js, i, ntokens, tokens, recordT);
      else if (mapName == "XALT_measureT")
        processTable(name,js, i, ntokens, tokens, measureT);
    }
  free(tokens);
}

void parseLinkJsonStr(const char* name, std::string& jsonStr, Vstring& linklineA, Table& resultT, std::vector<Libpair>& libA, Set& funcSet)
{
  jsmn_parser parser;
  jsmntok_t*  tokens;
  int         maxTokens = 1000;

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
      fprintf(stderr,"(5) Bad link.*.json file: %s\n",name);
      exit(1);
    }

  int i = 1;
  while (i < ntokens)
    {
      if (tokens[i].type != JSMN_STRING )
        {
          fprintf(stderr,"(6) Bad file(%s): i: %d, Type: %d\n", name, i, tokens[i].type);
          exit(1);
        }
      std::string mapName(js, tokens[i].start, tokens[i].end - tokens[i].start); ++i;
      if (mapName == "function")
         processSet(name, js, i, ntokens, tokens, funcSet);
      else if (mapName == "linkA")
        processLibA(name, js, i, ntokens, tokens, libA);
      else if (mapName == "link_line")
        processArray(name,js, i, ntokens, tokens, linklineA);
      else if (mapName == "resultT")
          processTable(name,js, i, ntokens, tokens, resultT);
    }
  free(tokens);
}

void parseCompTJsonStr(const char* name, std::string& jsonStr, std::string& compiler, std::string& compilerPath,
                       Vstring& linklineA)
{
  jsmn_parser parser;
  jsmntok_t*  tokens;
  int         maxTokens = 1000;

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
      fprintf(stderr,"(5) Bad run.*.json file: %s\n",name);
      exit(1);
    }

  int i = 1;
  while (i < ntokens)
    {
      if (tokens[i].type != JSMN_STRING )
        {
          fprintf(stderr,"(6) Bad file(%s): i: %d, Type: %d\n", name, i, tokens[i].type);
          exit(1);
        }
      std::string mapName(js, tokens[i].start, tokens[i].end - tokens[i].start); ++i;
      if (mapName == "compiler")
        processValue(name,js, i, ntokens, tokens, compiler);
      else if (mapName == "compilerPath")
        processValue(name,js, i, ntokens, tokens, compilerPath);
      else if (mapName == "link_line")
        processArray(name,js, i, ntokens, tokens, linklineA);
    }
  free(tokens);
}
