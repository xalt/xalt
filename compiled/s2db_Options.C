#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "s2db_Options.h"

#define HERE fprintf(stderr,"%s:%d\n",__FILE__, __LINE__)
s2db_Options::s2db_Options(int argc, char** argv)
  : m_timer(false),           m_syslogFn("unknown"),    m_leftoverFn("leftover.log"),
    m_rmapD("unknown"),       m_confFn("xalt_db.conf")
{
  int   c;
  char *p;

  while(1)
    {
      int option_index       = 0;
      static struct option long_options[] = {
        {"confFn",      optional_argument, NULL, 'c'},
        {"reverseMapD", required_argument, NULL, 'R'},
        {"syslog",      required_argument, NULL, 's'},
        {"leftover",    optional_argument, NULL, 'l'},
        {"timer",       optional_argument, NULL, 't'},
        {0,             0,                 0,     0 }
      };
      
      c = getopt_long(argc, argv, "c:R:s:l:t",
		      long_options, &option_index);
      
      if (c == -1)
        break;

      switch(c)
	{
	case 'c':
          if (optarg)
            m_confFn = optarg;
	  break;
	case 'R':
          if (optarg)
            m_rmapD = optarg;
	  break;
	case 's':
          if (optarg)
            m_syslogFn = optarg;
	  break;
	case 'l':
          if (optarg)
            m_leftoverFn = optarg;
	  break;
	case 't':
          m_timer  = true;
	  break;
	case '?':
	  printf("Huh?\n");
	  break;
	default:
	  printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
}
