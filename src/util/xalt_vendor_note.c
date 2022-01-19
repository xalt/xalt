#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <link.h>
#include <gelf.h>
#include "xalt_config.h"
#include "xalt_c_utils.h"
#include "xalt_vendor_note.h"
#include "xalt_debug_macros.h"

// Use the real xalt_vendor_note() only with 64bit executables not 32bit.
#ifndef WITH_32BIT

static int xalt_tracing = 0;

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


/**
 * Read a "vendor specific ELF note".
 * Only documentation I could find: http://www.netbsd.org/docs/kernel/elf-notes.html
 * The signature is in the format:
 *                4 bytes
 * +-----------------------------------+
 * |             name_size             |
 * +-----------------------------------+
 * |             desc_size             |
 * +-----------------------------------+
 * |               type                |
 * +-----------------------------------+
 * |               name                |
 * +-----------------------------------+
 * | ver |       watermark             |
 * +-----------------------------------+
 *
 * All values are padded to 4 byte boundaries.
 * XALT version is 1 byte, The watermark is a array of character strings.
 */


static int read_watermark(const void *note, char **ret_watermark)
{
  const elf_note *elf_note = note;

  if (elf_note->type == XALT_ELF_NOTE_TYPE)
    {
      const vendor_note *xalt_note = (const struct vendor_note *)&(elf_note->data);
      if (xalt_note->version == XALT_STAMP_SUPPORTED_VERSION)
        {
          int    len;
          char * watermark = (char *) XMALLOC(sizeof(char)*elf_note->desc_size);
          const char * p   = &xalt_note->note[0];
          char * q         = watermark;
      
          for (; *p != '\0'; p += len + 1)
            {
              len   = strlen(p);
              memcpy(q,p,len);
              q[len] = '.';
              q += len+1;
            }
          *q = '\0';
          *ret_watermark = watermark;
        }
    }

  return elf_note->desc_size + elf_note->name_size + 12;
}

static int handle_program_header(struct dl_phdr_info *info, __attribute__((unused))size_t size, void *data)
{
  int j;
  char *   watermark = NULL;

  char **pp = (char **) data;
  if (*pp)
    // I already have a watermark so quit looking!
    return 0;

  const char* so_name = info->dlpi_name;
  if (*so_name != '\0')
    // Do not need look because if *so_name is non-NULL then it is a shared library
    // and I don't want to know about any watermark there.
    return 0;
	
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
		    goto found;
                }
            }
        }
    }
 found:
  *pp = watermark;
  return 0;
}

bool xalt_vendor_note(char ** watermark, int xalt_tracingIn)
{
  *watermark = NULL;
  xalt_tracing = xalt_tracingIn;
  dl_iterate_phdr(handle_program_header, (void *)watermark);
  if (*watermark == NULL)
    return 0;
  return 1;
}

#else
bool xalt_vendor_note(char ** watermark, int xalt_tracingIn)
{
  *watermark = NULL;
  return 0;
}
#endif
