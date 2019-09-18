#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uuid/uuid.h"

int main()
{
  char   my_uuid_str[37];
  uuid_t uuid;
  uuid_generate_random(uuid);
  uuid_unparse_lower(uuid, my_uuid_str);

  printf("%s\n", my_uuid_str);
}
