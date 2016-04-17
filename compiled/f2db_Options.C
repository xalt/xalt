#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "f2db_Options.h"

#define HERE fprintf(stderr,"%s:%d\n",__FILE__, __LINE__)
f2db_Options::f2db_Options(int argc, char** argv)
  : m_delete(false),          m_report(false),          m_timer(false), 
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
        {"delete",      optional_argument, NULL, 'e'},
        {"report",      optional_argument, NULL, 'r'},
        {"timer",       optional_argument, NULL, 't'},
        {0,             0,                 0,     0 }
      };
      
      c = getopt_long(argc, argv, "c:R:ert",
		      long_options, &option_index);
      
      if (c == -1)
        break;

      switch(c)
	{
	case 'e':
          m_delete = true;
	  break;
	case 'r':
          m_report = true;
	  break;
	case 't':
          m_timer  = true;
	  break;
	case 'c':
          fprintf(stderr,"optarg: %s\n",optarg);
	  m_confFn.assign(optarg);
	  break;
	case 'R':
	  m_rmapD = optarg;
	  break;
	case '?':
	  printf("Huh?\n");
	  break;
	default:
	  printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
}
