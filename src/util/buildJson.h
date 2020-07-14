#ifndef BUILD_JSON_H
#define BUILD_JSON_H

#include "xalt_types.h"
#include "utarray.h"
#include "xalt_obfuscate.h"

#ifdef __cplusplus
extern "C"
{
#endif

  typedef enum
    {
     Json_TABLE,
     Json_ARRAY
    } Json_kind_t;
  
  typedef struct
  {
    const char* m_final;
    UT_array*   m_s;
  } Json_t;
  
  void json_init(Json_kind_t kind, Json_t* json);
  void json_fini(Json_t* json, char** p_result);
  void json_add_char_str( Json_t* json, const char* sep, const char* name, const char *   value);
  void json_add_json_str( Json_t* json, const char* sep, const char* name, const char *   value);
  void json_add_int(      Json_t* json, const char* sep, const char* name, int            value);
  void json_add_double(   Json_t* json, const char* sep, const char* name, double         value);
  void json_add_SET(      Json_t* json, const char* sep, const char* name, SET_t*         libT);
  void json_add_S2S(      Json_t* json, const char* sep, const char* name, S2S_t*         value);
  void json_add_S2D(      Json_t* json, const char* sep, const char* name, S2D_t*         value);
  void json_add_ptA(      Json_t* json, const char* sep, const char* name, processTree_t* ptA);
  void json_add_array(    Json_t* json, const char* sep, const char* name, int n, const char** A);
  void json_add_utarray(  Json_t* json, const char* sep, const char* name, UT_array** A);

#ifdef __cplusplus
}
#endif

#endif //BUILD_JSON_H
