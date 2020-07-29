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

void free_SET(SET_t** libT)
{
  SET_t *entry, *tmp;
  HASH_ITER(hh, *libT, entry, tmp)
    {
      HASH_DEL(*libT, entry);
      utstring_free(entry->key);
      my_free(entry, sizeof(SET_t));
    }
  libT = NULL;
}

void free_S2D(S2D_t** userDT)
{
  S2D_t *entry, *tmp;
  HASH_ITER(hh, *userDT, entry, tmp)
    {
      HASH_DEL(*userDT, entry);
      utstring_free(entry->key);
      my_free(entry, sizeof(S2D_t));
    }
  userDT = NULL;
}

void free_S2S(S2S_t** userT)
{
  S2S_t *entry, *tmp;
  HASH_ITER(hh, *userT, entry, tmp)
    {
      HASH_DEL(*userT, entry);
      utstring_free(entry->key);
      utstring_free(entry->value);
      my_free(entry, sizeof(S2S_t));
    }
  userT = NULL;
}
