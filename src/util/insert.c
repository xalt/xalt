#include "insert.h"

void insert_key_double(S2D_t** userDT, const char* name, double value)
{
  S2D_t* entry = (S2D_t *) malloc(sizeof(S2D_t));
  utstring_new(entry->key);
  utstring_bincpy(entry->key, name, strlen(name));

  entry->value = value;

  HASH_ADD_KEYPTR(hh, *userDT, utstring_body(entry->key), utstring_len(entry->key), entry);
}

void insert_key_string(S2S_t** userT, const char* name, const char* value)
{
  S2S_t* entry = (S2S_t *) malloc(sizeof(S2S_t));
  utstring_new(entry->key);
  utstring_bincpy(entry->key, name, strlen(name));

  utstring_new(entry->value);
  utstring_bincpy(entry->value, value, strlen(value));

  HASH_ADD_KEYPTR(hh, *userT, utstring_body(entry->key), utstring_len(entry->key), entry);
}

void insert_key_SET(SET_t** libT, const char* name)
{
  SET_t* entry = (SET_t *) malloc(sizeof(SET_t));
  utstring_new(entry->key);
  utstring_bincpy(entry->key, name, strlen(name));

  HASH_ADD_KEYPTR(hh, *libT, utstring_body(entry->key), utstring_len(entry->key), entry);
}
