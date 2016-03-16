#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "Options.h"
#include "run_submission.h"
#include "xalt_config.h"

double convert_double(const char* name, const char* s)
{
  char  *p;
  double v;

  v = strtod(s, &p);
  if (p == s || *p)
    {
      fprintf(stderr,"For option: \"%s\", unable to parse: \"%s\"\n", name, s);
      exit(1);
    }
  return v;
}

long convert_long(const char* name, const char* s)
{
  char *p;
  long  v;

  v = strtol(s, &p, 10);
  if (p == s || *p)
    {
      fprintf(stderr,"For option: \"%s\", unable to parse: \"%s\"\n", name, s);
      exit(1);
    }
  return v;
}

Options::Options(int argc, char** argv)
  : m_start(0.0), m_end(0.0), m_ntasks(1L),
    m_syshost("unknown"),     m_uuid("unknown"),
    m_exec("unknown"),        m_userCmdLine("[]"),
    m_exec_type("unknown"),   m_confFn("xalt_db.conf")
{
  int   c;
  char *p;

  while(1)
    {
      int option_index       = 0;
      static struct option long_options[] = {
        {"start",   required_argument, NULL, 's'},
        {"end",     required_argument, NULL, 'e'},
        {"syshost", required_argument, NULL, 'h'},
        {"exec",    required_argument, NULL, 'x'},
        {"ntasks",  required_argument, NULL, 'n'},
        {"uuid",    required_argument, NULL, 'u'},
        {"confFn",  optional_argument, NULL, 'c'},
        {0,         0,                 0,     0 }
      };
      
      c = getopt_long(argc, argv, "c:s:e:h:x:n:u:",
		      long_options, &option_index);
      
      if (c == -1)
	break;

      switch(c)
	{
	case 's':
          m_start = convert_double("start", optarg);
	  break;
	case 'e':
	  m_end = convert_double("end", optarg);
	  break;
	case 'c':
	  m_confFn = optarg;
	  break;
	case 'h':
	  m_syshost = optarg;
	  break;
        case 'x':
	  m_exec = optarg;
	  break;
	case 'n':
	  m_ntasks = convert_long("ntasks", optarg);
	  break;
        case 'u':
	  m_uuid = optarg;
	  break;
	case '?':
	  printf("Huh?\n");
	  break;
	default:
	  printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
  
  if (optind < argc)
    m_userCmdLine = argv[optind];


  if (m_exec != "unknown")
    {
      std::vector<std::string> result;
      std::string              cmd;

      cmd = PATH_TO_PRGM_FILE " " + m_exec;
      capture(cmd, result);
      if (result[0].find("script") != std::string::npos ||
          result[0].find("text")   != std::string::npos)
        m_exec_type = "script";
      else
        m_exec_type = "binary";
    }
}
