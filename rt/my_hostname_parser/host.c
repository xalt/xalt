#include <xalt_obfuscate.h>
#include <stdio.h>
int return_keep();

int my_hostname_parser(const char *hostname)
{
  printf(stderr,"In my_hostname_parser\n");
  return return_keep();
}

void my_hostname_parser_cleanup()
{
}
