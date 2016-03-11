#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define HERE fprintf(stderr,"%s:%d\n",__FILE__,__LINE__);fflush(stderr)
const char *qcharA[] = { "\\u0000","\\u0001","\\u0002","\\u0003","\\u0004","\\u0005","\\u0006","\\u0007",
		       	 "\\b"    ,"\\t"    ,"\\n"    ,"\\u000b","\\f"    ,"\\r"    ,"\\u000e","\\u000f",
		       	 "\\u0010","\\u0011",    "\\r","\\u0013","\\u0014","\\u0015","\\u0016","\\u0017",
		       	 "\\u0018","\\u0019","\\u001a","\\u001b","\\u001c","\\u001d","\\u001e","\\u001f",		       
		       	 " "      ,      "!",     "\\\""};

static char * buff;
static unsigned int sz = 0;


const char* xalt_quotestring(const char* input)
{
  const char   *p = input;
  char         *s;
  unsigned char a,b,c,d;
  int high, low, len, currSz;
  len    = strlen(input);
  currSz = 3*len+1;
  if (sz < currSz)
    {
      sz = currSz;
      buff = (char *) malloc(sz);
    }
  s = buff;

  for (; *p; ++p)
    {
      a = *p;
      if (a < 0x023)
	{
	  const char *r = qcharA[a];
	  len = strlen(r);
	  memcpy(s,r,len);
	  s += len;
	}
      else if (a == '\\')
	{
	  memcpy(s,"\\\\",2);
	  s += 2;
	}
      else if (a < 0x07f)
	*s++ = a;
      else
        {
          int value;
	  b = *++p;
          if (0xc0 <= a && a < 0xdf && b >= 0x80)
            value = (a - 0xc0) * 0x40 + b - 0x80;
          else if ( 0xe0 <= a && a <= 0xef && b >= 0x80 && (c = *++p) >= 0x80)
            value = ((a - 0xe0) * 0x40 + b - 0x80) * 0x40 + c - 0x80;
          else if (  0xf0 <= a && a <= 0xf7 && b >= 0x80 && c >= 0x80 && (d = *++p) >= 0x80 )
            value = (((a - 0xf0) * 0x40 + b - 0x80) * 0x40 + c - 0x80) * 0x40 + d - 0x80;
          else
            value = 0;
          if (value <= 0xffff)
            {
              sprintf(s,"\\u%.4x",value);
              s += 6;
            }
          else if (value <= 0x10ffff)
            {
              value -= 0x10ffff;
              high   = 0xD800 + (value/0x400);
              low    = 0xDC00 + (value % 0x400);
              sprintf(s,"\\u%.4x\\u%.4x",high,low);
              s += 12;
            }
        }
    }
  *s = '\0';
  return buff;
}
