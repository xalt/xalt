#include <stdlib.h>
#include <string.h>
#include "buildJson.h"
#include "processTree.h"
#include "xalt_c_utils.h"
#include "utlist.h"
#include "xalt_quotestring.h"
#include "xalt_debug_macros.h"

static const char* dquote      = "\"";
static const char* s_colon     = "\":\"";
static const char* n_colon     = "\":";
static const char* t_colon     = "\":{";
static const char* a_colon     = "\":[";
static const char* end_bracket = "]";
static const char* end_brace   = "}";
static const char* bracket     = "[";
static const char* bracketQ    = "[\"";
static const char* brace       = "{";

static const char* blank0      = "";
static const char* comma       = ",";

void json_init(Json_kind_t kind, Json_t* json)
{
  const char* init;

  if (kind == Json_ARRAY)
    {
      json->m_final = end_bracket;
      init          = bracket;
    }
  else
    {
      json->m_final = end_brace;
      init          = brace;
    }

  utarray_new(json->m_s, &ut_str_icd);
  utarray_push_back(json->m_s, &init);
}

void json_fini(Json_t* json, char** p_result)
{
  utarray_push_back(json->m_s, &json->m_final);

  int sz = 0;
  char **p = NULL;
  while( (p = (char **)utarray_next(json->m_s, p)) != NULL)
    sz += strlen(*p);

  char* result = (char*) XMALLOC(sizeof(char)*(sz+1));
  p = NULL;
  char* q = result;
  while( (p = (char **)utarray_next(json->m_s, p)) != NULL)
    {
      int len = strlen(*p);
      memcpy(q, *p, len);
      q += len;
    }
  *q = '\0';
  *p_result = result;
  utarray_free(json->m_s);
}

void json_add_char_str(Json_t* json, const char* sep, const char* name, const char * value)
{
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &s_colon);
  utarray_push_back(json->m_s, &value);
  utarray_push_back(json->m_s, &dquote);
}

void json_add_json_str(Json_t* json, const char* sep, const char* name, const char * value)
{
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &n_colon);
  utarray_push_back(json->m_s, &value);
}

void json_add_int(Json_t* json, const char* sep, const char* name, int value)
{
  char buf[30];
  char *s = &buf[0];
  sprintf(&buf[0],"%d",value);
  
  
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &n_colon);
  utarray_push_back(json->m_s, &s);
}

void json_add_double(Json_t* json, const char* sep, const char* name, double value)
{
  char buf[30];
  char *s = &buf[0];
  sprintf(&buf[0],"%g",value);

  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &n_colon);
  utarray_push_back(json->m_s, &s);
}

void json_add_S2S(Json_t* json, const char* sep, const char* name, S2S_t* value)
{
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &t_colon);

  const char* my_sep = blank0;
  S2S_t *entry, *tmp;

  HASH_ITER(hh, value, entry, tmp)
    {
      const char* my_value = xalt_quotestring(utstring_body(entry->value));

      utarray_push_back(json->m_s, &my_sep);
      utarray_push_back(json->m_s, &dquote);
      utarray_push_back(json->m_s, &utstring_body(entry->key));
      utarray_push_back(json->m_s, &s_colon);
      utarray_push_back(json->m_s, &my_value);
      utarray_push_back(json->m_s, &dquote);
      my_sep = comma;
    }
  utarray_push_back(json->m_s, &end_brace);
  xalt_quotestring_free();
}

void json_add_SET(Json_t* json, const char* sep, const char* name, SET_t* setT)
{
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &a_colon);

  const char* my_sep = blank0;
  SET_t *entry, *tmp;

  HASH_ITER(hh, setT, entry, tmp)
    {
      utarray_push_back(json->m_s, &my_sep);
      utarray_push_back(json->m_s, &dquote);
      utarray_push_back(json->m_s, &utstring_body(entry->key));
      utarray_push_back(json->m_s, &dquote);
      my_sep = comma;
    }
  utarray_push_back(json->m_s, &end_bracket);
}

void json_add_libT(Json_t* json, const char* sep, const char* name, SET_t* libT)
{
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &a_colon);

  const char* zero_str = "\",\"0\"]";

  const char* my_sep = blank0;
  SET_t *entry, *tmp;

  HASH_ITER(hh, libT, entry, tmp)
    {
      utarray_push_back(json->m_s, &my_sep);
      utarray_push_back(json->m_s, &bracketQ);
      const char* q = xalt_quotestring(utstring_body(entry->key));
      utarray_push_back(json->m_s, &q);
      utarray_push_back(json->m_s, &zero_str);
      my_sep = comma;
    }
  utarray_push_back(json->m_s, &end_bracket);
  xalt_quotestring_free();
}

void json_add_S2D(Json_t* json, const char* sep, const char* name, S2D_t* value)
{
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &t_colon);

  const char* my_sep = blank0;
  S2D_t *entry, *tmp;
  UT_string* buf;
  utstring_new(buf);

  HASH_ITER(hh, value, entry, tmp)
    {
      utarray_push_back(json->m_s, &my_sep);
      utarray_push_back(json->m_s, &dquote);
      utarray_push_back(json->m_s, &utstring_body(entry->key));
      utarray_push_back(json->m_s, &n_colon);

      utstring_clear(buf);
      utstring_printf(buf, "%.4f", entry->value);
      utarray_push_back(json->m_s, &utstring_body(buf));
      my_sep = comma;
    }
  utarray_push_back(json->m_s, &end_brace);
  utstring_free(buf);
}

// "processTree":
//     [
//        { "cmd_name":"foo", "cmd_path":"/path/to/foo", "cmdlineA":[ "./foo","-a"]},
//        { "cmd_name":"bar", "cmd_path":"/path/to/bar", "cmdlineA":[ "./bar","-b"]}
//     ],

void json_add_ptA(Json_t* json, const char* sep, const char* name, processTree_t* ptA)
{
  char           buf[30];
  char*          s        = &buf[0];
  const char*    cmd_name = "{\"cmd_name\":\"";
  const char*    cmd_path = "\",\"cmd_path\":\"";
  const char*    pid_str  = "\",\"pid\":";
  const char*    cmdlineA = ",\"cmdlineA\":[";
  const char*    closeStr = "]}";

  const char*    my_sep   = blank0;
  char**         p;
  const char*    q;
  processTree_t* entry;

  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &a_colon);
  
  DL_FOREACH(ptA, entry)
    {
      utarray_push_back(json->m_s, &my_sep);
      my_sep = comma;
      utarray_push_back(json->m_s, &cmd_name);
      q = xalt_quotestring(utstring_body(entry->m_name));
      utarray_push_back(json->m_s, &q);

      utarray_push_back(json->m_s, &cmd_path);
      q = xalt_quotestring(utstring_body(entry->m_path));
      utarray_push_back(json->m_s, &q);

      utarray_push_back(json->m_s, &pid_str);
      sprintf(&buf[0], "%d", entry->m_pid);
      utarray_push_back(json->m_s, &s);
      utarray_push_back(json->m_s, &cmdlineA);
      
      const char*    inner_sep = blank0;

      p = NULL;
      while ( (p=(char**)utarray_next(entry->m_cmdlineA, p)) != NULL)
	{
	  utarray_push_back(json->m_s, &inner_sep);
	  utarray_push_back(json->m_s, &dquote);
	  const char* q = xalt_quotestring(*p);
	  utarray_push_back(json->m_s, &q);
	  utarray_push_back(json->m_s, &dquote);
	  inner_sep = comma;
	}
      utarray_push_back(json->m_s, &closeStr);
    }
  utarray_push_back(json->m_s, &end_bracket);
  xalt_quotestring_free();
}

void json_add_array(Json_t* json, const char* sep, const char* name, int n, const char** A)
{
  int i;
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &a_colon);
  
  const char*    my_sep   = blank0;
  for (i = 0; i < n; ++i)
    {
      utarray_push_back(json->m_s, &my_sep);
      utarray_push_back(json->m_s, &dquote);
      const char* q = xalt_quotestring(A[i]);
      utarray_push_back(json->m_s, &q);
      utarray_push_back(json->m_s, &dquote);
      my_sep = comma;
    }
  utarray_push_back(json->m_s, &end_bracket);
  xalt_quotestring_free();
}

void json_add_utarray(  Json_t* json, const char* sep, const char* name, UT_array* A)
{
  utarray_push_back(json->m_s, &sep);
  utarray_push_back(json->m_s, &dquote);
  utarray_push_back(json->m_s, &name);
  utarray_push_back(json->m_s, &a_colon);
  
  char**         p        = NULL;
  const char*    my_sep   = blank0;
  while( (p = (char **)utarray_next(A, p)) != NULL)
    {
      utarray_push_back(json->m_s, &my_sep);
      utarray_push_back(json->m_s, &dquote);
      const char* q = xalt_quotestring(*p);
      utarray_push_back(json->m_s, &q);
      utarray_push_back(json->m_s, &dquote);
      my_sep = comma;
    }
  utarray_push_back(json->m_s, &end_bracket);
  xalt_quotestring_free();
}
