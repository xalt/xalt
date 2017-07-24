#include <stdio.h>
#include <stdlib.h>
#define DFLT "Not defined!"
int main(int argc, char* argv[])
{
  char *v = getenv("XALT_RUN_UUID");
  if(!v)
    v = DFLT;
  printf("XALT_RUN_UUID: %s\n",v);

  v = getenv("XALT_DATE_TIME");
  if(!v)
    v = DFLT;
  printf("XALT_DATE_TIME: %s\n",v);
    
  
  return 0;
}
