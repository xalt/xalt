#include "buildXALTRecordT.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int ascending_sort(void *_a, void *_b)
{
  S2S_t *a = (S2S_t*)_a;
  S2S_t *b = (S2S_t*)_b;
  return strcmp(utstring_body(a->key),utstring_body(b->key));
}

int main(int argc, char* argv[])
{
  const int   leftColWidth = 25;
  const char* blanks       = "                                        ";

  if (argc < 2)
    {
      fprintf(stderr,"Usage: xalt_extract_record a.out\n");
      exit(1);
    }

  const char* exec_path = argv[1];

  S2S_t* recordT   = NULL;
  char*  watermark = NULL;
  extractXALTRecordString(exec_path, &watermark);
  buildXALTRecordT(watermark, &recordT);

  if (recordT == NULL)
    {
      printf("\nNo XALT Watermark\n");
      exit(1);
    }

  printf("****************************************\n");
  printf("XALT Watermark: %s\n", exec_path);
  printf("****************************************\n");

  HASH_SRT(hh, recordT, ascending_sort);
  S2S_t* entry;

  for (entry = recordT; entry != NULL; entry = (S2S_t*) entry->hh.next)
    {
      int my_len = leftColWidth - utstring_len(entry->key);
      printf("%s%.*s%s\n", utstring_body(entry->key), my_len, blanks, utstring_body(entry->value));
    }
  return 0;
}
