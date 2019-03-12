#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <link.h>
#include <gelf.h>

#define XALT_ELF_NOTE_TYPE    (0x746c6158)
#define XALT_STAMP_SUPPORTED_VERSION (0x02)
/* data structures */
struct elf_note {
  int32_t name_size;
  int32_t desc_size;
  int32_t type;
  uint8_t data[1];
} __attribute__((packed));
typedef struct elf_note elf_note;

struct vendor_note {
  char    name[8]; /* "XALT" plus null terminator, plus padding to 4 byte boundary */
  uint8_t version;
  char    note[1];
} __attribute__((packed));
typedef struct vendor_note vendor_note;

int read_watermark(const void *note, char **ret_watermark)
{
  const elf_note *elf_note = note;

  if (elf_note->type == XALT_ELF_NOTE_TYPE)
    {
      const vendor_note *xalt_note = (const struct vendor_note *)&(elf_note->data);
      if (xalt_note->version == XALT_STAMP_SUPPORTED_VERSION)
        {
          int size = 0;
          int len;
          int icount = 0;

          char * watermark = (char *) malloc(sizeof(char)*elf_note->desc_size);
          const char * p   = &xalt_note->note[0];
          char * q         = watermark;
      
          for (; *p != '\0'; p += len + 1)
            {
              len   = strlen(p);
              size += len + 1;
              memcpy(q,p,len);
              q[len] = '.';
              q += len+1;
            }
          *ret_watermark = watermark;
        }
    }

  return elf_note->desc_size + elf_note->name_size + 12;
}

int handle_program_header(struct dl_phdr_info *info, __attribute__((unused))size_t size, void *data)
{
  int j;
  char *   watermark = NULL;
  for (j = 0; j < info->dlpi_phnum; j++)
    {
      GElf_Phdr *program_header= (GElf_Phdr *)&(info->dlpi_phdr[j]);
      if (program_header->p_type != PT_NULL && program_header->p_type == PT_NOTE)
        {
          uint8_t *notes = (uint8_t *)(info->dlpi_addr + program_header->p_vaddr);
          if (notes != NULL)
            {
              uint32_t offset    = 0;
              while (offset < program_header->p_memsz)
                {
                  offset += read_watermark(notes + offset, &watermark);
                  if (watermark)
                    break;
                }
              if (watermark)
                break;
            }
        }
    }
  char **pp = (char **) data;
  *pp = watermark;
}

__attribute__((constructor)) void walk_so()
{
  char * watermark;
  dl_iterate_phdr(handle_program_header, (void *)&watermark);

  printf("watermark: %s\n", watermark);

}
                  
