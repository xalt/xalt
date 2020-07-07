#include <string.h>
#include "buildXALTRecordT.h"
#include <stdio.h>
#include <stdbool.h>

void buildXALTRecordT(char* watermark, S2S_t** recordT)
{
  char* start = watermark;
  while(1)
    {
      char *p = strchr(start,'<');
      if (p == NULL)
	break;
      p++;
      char *s1 = p;
      char *s2 = strstr(p,">%%");

      S2S_t* entry = (S2S_t *) malloc(sizeof(S2S_t));

      utstring_new(entry->key);
      utstring_bincpy(entry->key, s1, s2-s1);

      char *s3 = strstr(s2,"%%.");

      utstring_new(entry->value);

      p = s2+3;

      *s3 = '\0';

      while(p < s3)
	{
	  char* s4 = strstr(p,"_%_%_");
	  if (s4 != NULL)
	    {
	      utstring_bincpy(entry->value, p, s4 - p);
	      utstring_bincpy(entry->value, " ", 1);
	      p = s4+5;
	    }
	  else
	    {
	      utstring_bincpy(entry->value, p, s3 - p);
	      break;
	    }
	}
      HASH_ADD_KEYPTR(hh, *recordT, utstring_body(entry->key), utstring_len(entry->key), entry);

      start = s3+1;
    }
}

#define DATA_SIZE 1024

static void capture(const char* cmdline, UT_array** p_resultA)
{
  FILE* fp;
  fp = popen(cmdline,"r");
  UT_array* resultA = *p_resultA;

  char* eof;
  char data[DATA_SIZE];
  while( (eof = fgets(data, DATA_SIZE, fp)) != NULL)
    utarray_push_back(resultA, data);
  pclose(fp);
}


  /*
   * The results of the objdump command looks like this (after the "Contents ..." line)
   *  0000 58414c54 5f4c696e 6b5f496e 666f0000  XALT_Link_Info..
   *  0010 00000000 00003c58 414c545f 56657273  ......<XALT_Vers
   *  0020 696f6e3e 2525302e 36252500 3c427569  ion>%%0.6%%.<Bui
   *  0030 6c642e53 7973686f 73743e25 25766d69  ld.Syshost>%%vmi
   *  0040 6a6f2525 003c4275 696c642e 636f6d70  jo%%.<Build_comp
   *  0050 696c6572 3e252567 63632525 003c4275  iler>%%gcc%%.<Bu
   *  0060 696c642e 636f6d70 696c6572 50617468  ild.compilerPath
   *  0070 3e25252f 7573722f 62696e2f 6763632d  >%%/usr/bin/gcc-
   *  0080 342e3825 25003c42 75696c64 2e4f533e  4.8%%.<Build_OS>
   *  0090 25254c69 6e75785f 255f255f 332e3133  %%Linux_%_%_3.13
   *  00a0 2e302d37 372d6765 6e657269 63252500  .0-77-generic%%.
   *  00b0 3c427569 6c642e55 7365723e 25256d63  <Build_User>%%mc
   *  00c0 6c617925 25003c42 75696c64 2e555549  lay%%.<Build_UUI
   *  00d0 443e2525 66643466 62393061 2d636133  D>%%fd4fb90a-ca3
   *  00e0 632d3462 32322d38 6666382d 65363261  c-4b22-8ff8-e62a
   *  00f0 65616535 39643439 2525003c 4275696c  eae59d49%%.<Buil
   *  0100 642e5965 61723e25 25323031 36252500  d.Year>%%2016%%.
   *  0110 3c427569 6c642e64 6174653e 25255475  <Build_date>%%Tu
   *  0120 655f255f 255f4665 625f255f 255f3233  e_%_%_Feb_%_%_23
   *  0130 5f255f25 5f30393a 35303a35 355f255f  _%_%_09:50:55_%_
   *  0140 255f3230 31362525 003c4275 696c642e  %_2016%%.<Build_
   *  0150 45706f63 683e2525 31343536 32343236  Epoch>%%14562426
   *  0160 35352e39 34252500 00000000 00000058  55.94%%........X
   *  0170 414c545f 4c696e6b 5f496e66 6f5f456e  ALT_Link_Info_En
   *  0180 6400                                 d.              
   */

bool extractXALTRecordString(const char* exec_path, char** watermark)
{

  // Convert exec_path to execQ where '"' are backslash quoted.

  int   len   = strlen(exec_path);
  char* execQ = (char *)malloc(len*3*sizeof(char));
  char* p     = exec_path;
  char* q     = execQ;
  char  Q     = '"';
  const char* QQ = "\\\"";

  while(*p)
    {
      if (*p == Q)
	{
	  memcpy(q, QQ, 2);
	  q += 2;
	  p++;
	}
      else
	*q++ = *p++;
    }
      
  UT_string* cmd;
  utstring_new(cmd);
  
  utstring_printf(cmd, "XALT_EXECUTABLE_TRACKING=no PATH=" XALT_SYSTEM_PATH
                       " objdump -s -j .xalt \"%s\" 2> /dev/null", execQ);
  
  free(execQ);
  utstring_free(cmd);

  UT_array* resultA;
  utarray_new(resultA, &ut_str_icd);
  capture(utstring_body(cmd), &resultA);

  bool  found = false;
  char **pp   = NULL;
  while( (pp= (char**) utarray_next(resultA, pp)) != NULL)
    {
      if (strcmp(*pp,"Contents of section") == 0)
	{
	  found = true;
	  break;
	}
    }
  if (! found)
    {
      *watermark = strdup("FALSE");
      return false;
    }

  UT_string* xaltRecord;
  utstring_new(xaltRecord);

  // We are now at the first line after the "Contents of ..."
  while( (pp= (char**) utarray_next(resultA, pp)) != NULL)
    {
      char q* = strstr(*pp, "  ");
      q       = strcspn(q," ");
      utstring_bincpy(xaltRecord, q, strlen(q));
    }

  utarray_free(resultA);
  
  *watermark = strdup(utstring_body(xaltRecord));
  utstring_free(xaltRecord);
  return true;
}




//int main()
//{
//
//  const char *w = "<XALT_VERSION>%%2.8.7%%.<Build.Syshost>%%jedrik%%.<Build.User>%%Robert_%_%_T._%_%_McLay%%.";
//  char *watermark = strdup(w);
//
//  S2S_t *recordT = NULL;
//
//  S2S_t *e, *tmp;
//
//  buildXALTRecordT(watermark, &recordT);
//  
//  HASH_ITER(hh, recordT, e, tmp)
//    {
//      printf("recordT[\"%s\"]:\"%s\";\n",utstring_body(e->key), utstring_body(e->value));
//    }
//    
//  return 0;
//}
