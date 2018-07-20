#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <vector>
#include "Options.h"
#include "capture.h"
#include "xalt_config.h"
#include "base64.h"

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
  : m_start(0.0), m_end(0.0), m_ntasks(1L), m_ngpus(0L),
    m_interfaceV(0L),         m_ppid(0L),
    m_syshost("unknown"),     m_uuid("unknown"),
    m_exec("unknown"),        m_userCmdLine("[]"),
    m_exec_type("unknown"),   m_confFn("xalt_db.conf")
{
  int   c;

  while(1)
    {
      int option_index       = 0;
      static struct option long_options[] = {
        {"interfaceV", required_argument, NULL, 'V'},
        {"start",      required_argument, NULL, 's'},
        {"end",        required_argument, NULL, 'e'},
        {"syshost",    required_argument, NULL, 'h'},
        {"exec",       required_argument, NULL, 'x'},
        {"ntasks",     required_argument, NULL, 'n'},
        {"ngpus",      required_argument, NULL, 'g'},
        {"uuid",       required_argument, NULL, 'u'},
        {"confFn",     required_argument, NULL, 'c'},
        {"ppid",       required_argument, NULL, 'p'},
        {"path",       required_argument, NULL, 'P'},
        {"prob",       required_argument, NULL, 'b'},
        {"ld_libpath", required_argument, NULL, 'L'},
        {0,         0,                 0,     0 }
      };
      
      c = getopt_long(argc, argv, "c:s:e:h:x:n:g:u:p:P:L:",
		      long_options, &option_index);
      
      if (c == -1)
	break;

      switch(c)
	{
        case 'V':
          if (optarg)
            m_interfaceV = (pid_t) convert_long("ppid", optarg);
          break;
        case 'p':
          if (optarg)
            m_ppid = (pid_t) convert_long("ppid", optarg);
          break;
        case 'b':
          if (optarg)
            m_probability = convert_double("prob", optarg);
          break;
	case 's':
          if (optarg)
            m_start = convert_double("start", optarg);
	  break;
	case 'e':
          if (optarg)
            m_end = convert_double("end", optarg);
	  break;
	case 'c':
          if (optarg)
            m_confFn = optarg;
	  break;
	case 'h':
          if (optarg)
            m_syshost = optarg;
	  break;
        case 'x':
          if (optarg)
            m_exec = optarg;
	  break;
	case 'n':
          if (optarg)
            m_ntasks = convert_long("ntasks", optarg);
	  break;
	case 'g':
          if (optarg)
            m_ngpus = convert_long("ngpus", optarg);
	  break;
        case 'u':
          if (optarg)
            m_uuid = optarg;
	  break;
        case 'P':
          if (optarg)
            m_path = optarg;
	  break;
        case 'L':
          if (optarg)
            m_ldLibPath = optarg;
	  break;
	case '?':
	  printf("Huh?\n");
	  break;
	default:
	  printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
  
  if (optind + 1 == argc && argv[optind][0] == '[')
    m_userCmdLine = argv[optind];
  else if (optind < argc)
    {
      if (m_interfaceV < 4)
        {
          m_userCmdLine = "[";
          for (int i = optind; i < argc; ++i)
            {
              m_userCmdLine += "\"";
              m_userCmdLine += argv[i];
              m_userCmdLine += "\",";
            }
          if (m_userCmdLine.back() == ',')
            m_userCmdLine.replace(m_userCmdLine.size()-1,1,"]");
        }
      else
        {
          int jLen;
          int len = strlen(argv[optind]);
          m_userCmdLine = reinterpret_cast<char*>(base64_decode(argv[optind], len, &jLen));
        }
    }

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
