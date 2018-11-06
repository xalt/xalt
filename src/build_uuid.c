#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xalt_obfuscate.h"
#include "build_uuid.h"
#include "xalt_fgets_alloc.h"
#include "xalt_config.h"
#include "uuid.h"
void build_uuid(char * my_uuid_str)
{
  uuid_t uuid;
  uuid_generate_random(uuid);
  uuid_unparse_lower(uuid, my_uuid_str);
}
