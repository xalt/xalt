#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "f2db_Options.h"

f2db_Options::f2db_Options(int argc, char** argv)
  : m_delete(false),          m_report(false),          m_timer(false), 
    m_rmapD("unknown"),       m_confFn("xalt_db.conf")
{
  int   c;

  while(1)
    {
      int option_index       = 0;
      static struct option long_options[] = {
        {"confFn",      required_argument, NULL, 'c'},
        {"reverseMapD", required_argument, NULL, 'R'},
        {"delete",      no_argument,       NULL, 'e'},
        {"report",      no_argument,       NULL, 'r'},
        {"timer",       no_argument,       NULL, 't'},
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
          if (optarg)
            m_confFn = optarg;
	  break;
	case 'R':
          if (optarg)
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
