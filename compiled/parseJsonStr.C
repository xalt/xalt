#include <stdio.h>
#include <stdlib.h>

#include "parseJsonStr.h"
#include "xalt_quotestring.h"

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

void processSet(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, Set& set)
{
  if (tokens[i].type != JSMN_ARRAY)
    {
      fprintf(stderr,"processSet for: %s, token type is not an array\n",name);
      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n",
              i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
      exit(1);
    }

  int iend = tokens[i].end;
  std::string value;
  const char  *p;

  ++i;
  while (i < ntokens)
    {
      if (tokens[i].start > iend)
        return;

      if (tokens[i].type != JSMN_STRING)
        {
          fprintf(stderr,"processSet for: %s, token type is not a string\n",name);
          exit(1);
        }
      p = xalt_unquotestring(&js[tokens[i].start],tokens[i].end - tokens[i].start); ++i;
      value.assign(p);
      set.insert(value);
    }
}
void processTable(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, Table& t)
{
  if (tokens[i].type != JSMN_OBJECT)
    {
      fprintf(stderr,"processTable for: %s, token type is not an object\n",name);
      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n",
              i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
      exit(1);
    }

  int iend = tokens[i].end;
  std::string key;
  std::string value;
  const char  *p;

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
      p = xalt_unquotestring(&js[tokens[i].start],tokens[i].end - tokens[i].start); ++i;
      key.assign(p);
      p = xalt_unquotestring(&js[tokens[i].start],tokens[i].end - tokens[i].start); ++i;
      value.assign(p);
      t[key] = value;
    }
}
void processA2JsonStr(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, std::string& value)
{
  value = "[";
  if (tokens[i].type != JSMN_ARRAY)
    {
      fprintf(stderr,"processA2JsonStr for: %s, token type is not an array\n",name);
      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n",
              i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
      exit(1);
    }

  int iend = tokens[i].end;

  ++i;
  while (i < ntokens)
    {
      if (tokens[i].start > iend)
        break;

      if (tokens[i].type != JSMN_STRING)
        {
          fprintf(stderr,"processSet for: %s, token type is not a string\n",name);
          exit(1);
        }
      value.append("\"");
      value.append(&js[tokens[i].start], tokens[i].end - tokens[i].start); ++i;
      value.append("\",");
    }
  if (value.back() == ',')
    value.replace(value.size()-1,1,"]");
  else
    value.append("]");
}

void processLibA(const char* name, const char* js, int& i, int ntokens, jsmntok_t* tokens, std::vector<Libpair>& libA)
{
  if (tokens[i].type != JSMN_ARRAY)
    {
      fprintf(stderr,"processTable for: %s, token type is not an object\n",name);
      fprintf(stderr, "%d: type: %d, start: %d, end: %d, size: %d, parent: %d\n",
              i, tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].parent);
      exit(1);
    }

  int iend = tokens[i].end;
  std::string lib;
  std::string sha1;
  const char  *p;

  ++i;
  while (i < ntokens)
    {
      if (tokens[i].start > iend)
        return;

      if (tokens[i].type != JSMN_ARRAY)
        {
          fprintf(stderr,"processLibA for %s: token type is not an array\n",name);
          exit(1);
        }
      i++;
      if (tokens[i].type != JSMN_STRING && tokens[i+1].type != JSMN_STRING)
        {
          fprintf(stderr,"processLibA for: %s, token types are not strings\n",name);
          exit(1);
        }
      
      p = xalt_unquotestring(&js[tokens[i].start],tokens[i].end - tokens[i].start); ++i;
      lib.assign(p);
      sha1.assign(&js[tokens[i].start],tokens[i].end - tokens[i].start);            ++i;
      Libpair libpair(lib,sha1);
      libA.push_back(libpair);
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
                     Table& envT, Table& userT, Table& recordT, std::vector<Libpair>& libA)
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
      fprintf(stderr,"(5) Bad run.*.json file: %s\n",name);
      exit(1);
    }

  int i = 1;
  while (i < ntokens)
    {
      if (tokens[i].type != JSMN_STRING )
        {
          fprintf(stderr,"(6) Bad file(%s): i: %d, Type: %d\n", name, i, tokens[i].type);
          fprintf(stderr,"%.*s\n",tokens[i].end - tokens[i].start, &js[tokens[i].start]);
          exit(1);
        }
      std::string mapName(js, tokens[i].start, tokens[i].end - tokens[i].start); ++i;
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
      else if (mapName == "xaltLinkT")
        processTable(name,js, i, ntokens, tokens, recordT);
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
