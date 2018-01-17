#include <uuid/uuid.h>
#include <stdio.h>

void rtm_build_uuid_str(char * my_uuid_str)
{
  uuid_t uuid;
  uuid_generate_random(uuid);
  uuid_unparse_lower(uuid, my_uuid_str);
}
int main()
{
   char uuid_str[37];
   rtm_build_uuid_str(uuid_str);

   printf("uuid: %s\n",uuid_str);
}
