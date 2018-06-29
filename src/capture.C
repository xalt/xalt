#include <stdio.h>
#include <stdlib.h>
#include "capture.h"
#define DATA_SIZE 1024

void capture(std::string& cmd, Vstring& result)
{
  FILE* fp;

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
}
