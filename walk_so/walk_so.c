#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <link.h>

int count_program_header(__attribute__((unused)) struct dl_phdr_info *info,
                         __attribute__((unused)) size_t size, void *data) 
{
  uint32_t *count = data;
  (*count)++;
  return 0;
}

int handle_program_header(struct dl_phdr_info *info, __attribute__((unused))size_t size, void *data)
{
  printf("name: %s, (%d segments)\n", info->dlpi_name, info-> dlpi_phnum);
}




void walk_so()
{
  uint32_t count = 0;
  dl_iterate_phdr(count_program_header, (void *)&count);
  printf("number of headers: %d\n", count);
  dl_iterate_phdr(handle_program_header, (void *)&count);
}
                  
int main(int argc, char* argv[])
{
  printf("Hello from walk_so!\n");
  walk_so();

  return 0;
}
