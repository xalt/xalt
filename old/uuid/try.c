#include <stdio.h>
#include <uuid/uuid.h>
int main(int argc, char* argv[])
{
  uuid_t t;
  char ch[36];
  
  uuid_generate(t);
  memset(ch, 0, 36);
  uuid_unparse(t, ch);

  printf("%s\n",ch);
  
  return 0;
}
