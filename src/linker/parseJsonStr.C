#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "xalt_quotestring.h"
#include "parseJsonStr.h"
#include "utarray.h"
#include "insert.h"

void processTable(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, S2S_t** t)
{
  if (tokens[i].type != JSMN_OBJECT)
    {
      fprintf(stderr,"processTable for: %s, token type is not an object\n",name);
      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n",
              i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
      exit(1);
    }

  int iend = tokens[i].end;
  const char* key;
  const char* value;

  ++i;
  while (i < ntokens)
    {
      if (tokens[i].start > iend)
        return;

      if (tokens[i].type != JSMN_STRING && tokens[i+1].type != JSMN_STRING)
        {
          fprintf(stderr,"processTable for: %s, token types are not strings\n",name);
          exit(1);
        }
      key   = xalt_unquotestring(&js[tokens[i].start],tokens[i].end - tokens[i].start); ++i;
      value = xalt_unquotestring(&js[tokens[i].start],tokens[i].end - tokens[i].start); ++i;
      insert_key_string(t, key, value);
    }
  xalt_quotestring_free();
}

void processArray(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, UT_array** p_vA)
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
  UT_array* vA = *p_vA;

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
      utarray_push_back(vA, &p);
    }
  xalt_quotestring_free();
}


//void processArray(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, Vstring& vA)
//{
//  if (tokens[i].type != JSMN_ARRAY)
//    {
//      fprintf(stderr,"processArray for: %s, token type is not an array\n",name);
//      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n",
//              i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
//      exit(1);
//    }
//
//  int iend = tokens[i].end;
//
//  std::string  value;
//  const char * p; 
//
//  ++i;
//  while (i < ntokens)
//    {
//      if (tokens[i].start > iend)
//        return;
//
//      if (tokens[i].type != JSMN_STRING)
//        {
//          fprintf(stderr,"processArray for: %s, token type is not a string\n",name);
//          exit(1);
//        }
//      p = xalt_unquotestring(&js[tokens[i].start],tokens[i].end - tokens[i].start); ++i;
//      value.assign(p);
//      vA.push_back(value);
//    }
//}

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


void parseCompTJsonStr(const char* name, std::string& jsonStr, std::string& compiler, std::string& compilerPath,
                       UT_array** linklineA)
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
  memset(tokens, 0, sizeof(jsmntok_t)*maxTokens);
  free(tokens);
}
