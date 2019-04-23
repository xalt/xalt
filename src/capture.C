#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "capture.h"
#define DATA_SIZE 1024

void replace_all(std::string& s1, const std::string from, const std::string to)
{
  for (std::string::size_type pos = 0;
       ( pos = s1.find(from, pos) ) != std::string::npos;
       pos += to.size() )
    s1.replace(pos, from.size(), to);
}

void capture(std::string& cmd, Vstring& result)
{
  FILE* fp;

  /* swallow stderr */
  int fd1, fd2;
  fflush(stderr);
  fd1 = dup(STDERR_FILENO);
  fd2 = open("/dev/null", O_WRONLY);
  dup2(fd2, STDERR_FILENO);
  close(fd2);

  fp = popen(cmd.c_str(), "r");
  if(!fp)
    {
      fprintf(stderr, "Could not open pipe for output for command: %s\n",cmd.c_str());
      exit(1);
    }

  char data[DATA_SIZE];

  char* eof;
  while((eof = fgets(data, DATA_SIZE, fp)) != NULL)
    result.push_back(data);

  pclose(fp);

  /* restore stderr */
  fflush(stderr);
  dup2(fd1, STDERR_FILENO);
  close(fd1);
}
