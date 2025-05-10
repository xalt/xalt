#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "build_uuid.h"

int main()
{
  char   my_uuid_str[37];
  build_uuid(my_uuid_str);
  printf("%s\n", my_uuid_str);
  build_uuid_cleanup();
  return 0;
}
