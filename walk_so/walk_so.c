#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <link.h>
#include <gelf.h>

#define OGRT_STAMP_SUPPORTED_VERSION (0x01)
/* data structures */
struct elf_note {
  int32_t name_size;
  int32_t desc_size;
  int32_t type;
  uint8_t data[1];
} __attribute__((packed));
typedef struct elf_note elf_note;

struct ogrt_note {
  char    name[8]; /* "OGRT" plus null terminator, plus padding to 4 byte boundary */
  uint8_t version;
  char    uuid[37];
} __attribute__((packed));
typedef struct ogrt_note ogrt_note;

struct so_infos {
  int32_t size;
  int32_t index;
  OGRT__SharedObject shared_objects[1];
};
typedef struct so_infos so_infos;


int count_program_header(__attribute__((unused)) struct dl_phdr_info *info,
                         __attribute__((unused)) size_t size, void *data) 
{
  uint32_t *count = data;
  (*count)++;
  return 0;
}

int read_signature(const void *note, uint8_t *ret_version, char **ret_signature)
{
  const elf_note *elf_note = note;

  if (elf_note->type == OGRT_ELF_NOTE_TYPE)
    {
      const ogrt_note *ogrt_note = (const struct ogrt_note *)&(elf_note->data);
      printf( "\tfound ogrt note with size %d!\n", elf_note->desc_size);
      printf( "[D] -> name %s (%10p)!\n", ogrt_note->name, ogrt_note->name);
      printf( "[D] -> version %u (%10p)!\n", ogrt_note->version, &ogrt_note->version);
      printf( "[D] -> signature %s (%10p)!", ogrt_note->uuid, ogrt_note->uuid);
      if (ogrt_note->version == OGRT_STAMP_SUPPORTED_VERSION)
        {
          *ret_version = ogrt_note->version;
          *ret_signature = (char *)ogrt_note->uuid;
        }
    }

  return elf_note->desc_size + elf_note->name_size + 12;
}

int handle_program_header(struct dl_phdr_info *info, __attribute__((unused))size_t size, void *data)
{
  printf("name: %s, (%d segments)\n", info->dlpi_name, info-> dlpi_phnum);

  int j;
  for (j = 0; < info->dlpi_phnum; j++)
    {
      GElf_Phdr *program_header= (GElf_Phdr *)&(info->dlpi_phdr[j]);
      if (program_header->p_type != PT_NULL && program_header->p_type == PT_NOTE)
        {
          uint8_t *notes = (uint8_t *)(info->dlpi_addr + program_header->p_vaddr);
          if (notes != NULL)
            {
              uint32_t offset    = 0;
              uint8_t  version   = 0;
              char *   signature = NULL;
              while (offset < program_header->p_memsz)
                {
                  offset += read_signature(notes + offset, &version, &signature);
                  printf("signature: %s\n",signature);
                }
            }
        }
    }
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
