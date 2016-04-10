#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "f2db_Options.h"

Options::Options(int argc, char** argv)
  : m_delete(false),          m_report(false),          m_timer(false), 
    m_rmapD("unknown"),       m_confFn("xalt_db.conf")
{
  int   c;
  char *p;

  while(1)
    {
      int option_index       = 0;
      static struct option long_options[] = {
        {"delete",      optional_argument, NULL, 'e'},
        {"report",      optional_argument, NULL, 'r'},
        {"timer",       optional_argument, NULL, 't'},
        {"reverseMapD", optional_argument, NULL, 'R'},
        {"confFn",      optional_argument, NULL, 'c'},
        {0,         0,                 0,     0 }
      };
      
      c = getopt_long(argc, argv, "ertR:c:",
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
	  m_confFn = optarg;
	  break;
	case 'R':
	  m_rmapT = optarg;
	  break;
	case '?':
	  printf("Huh?\n");
	  break;
	default:
	  printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
}
