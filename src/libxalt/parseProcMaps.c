#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "parseProcMaps.h"
#include "xalt_config.h"

#include "xalt_fgets_alloc.h"

/*
  The /proc/$PID/maps file looks something like this:

00848000-01615000 rw-p 00248000 08:01 270726                             /usr/bin/emacs25-x
01769000-020f0000 rw-p 00000000 00:00 0                                  [heap]
148c340d4000-148c34154000 rw-s 00000000 00:05 1474570                    /SYSV00000000 (deleted)
148c34154000-148c34156000 r-xp 00000000 08:01 524888                     /usr/lib/x86_64-linux-gnu/gdk-pixbuf-2.0/2.10.0/loaders/libpixbufloader-svg.so
148c34156000-148c34355000 ---p 00002000 08:01 524888                     /usr/lib/x86_64-linux-gnu/gdk-pixbuf-2.0/2.10.0/loaders/libpixbufloader-svg.so
148c34357000-148c3437b000 r--s 00000000 08:01 528130                     /usr/share/mime/mime.cache
148c3437b000-148c343cf000 r--p 00000000 08:01 919343                     /usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf
148c343cf000-148c343d0000 ---p 00000000 00:00 0 
148c347dc000-148c347dd000 rw-p 0000c000 08:01 524895                     /usr/lib/x86_64-linux-gnu/gio/modules/libdconfsettings.so
148c349de000-148c34a14000 r-xp 00000000 08:01 525344                     /usr/lib/x86_64-linux-gnu/gvfs/libgvfscommon.so.1.23
148c34c1b000-148c34c4c000 r-xp 00000000 08:01 527183                     /usr/lib/x86_64-linux-gnu/gio/modules/libgvfsdbus.so
148c3505b000-148c3525a000 ---p 0000b000 08:01 2752764                    /lib/x86_64-linux-gnu/libnss_files-2.27.so


So the steps for each line is to:

  a) find the leading "/"
  b) remove any lines that have "(deleted)" in them
  c) find the start of the filename (after the last "/")
  d) search for an ".so" in the file
  e) Make sure that the .so end the file name or it is .so.1.23.1
  f) remove libxalt_init.so
  g) remove trailing newline.
*/

void parseProcMaps(pid_t my_pid, SET_t** libT)
{
  char *   buf  = NULL;
  size_t   sz   = 0;
  char *   fn;
  
  (void) asprintf(&fn, "/proc/%d/maps", my_pid);
  FILE* fp    = fopen(fn,"r");
  if (!fp)
    return;

  while (xalt_fgets_alloc(fp, &buf, &sz))
    {
      // Step a:
      // find the start of the file name
      char *p = strstr(buf,"    /");
      if (p == NULL)
        continue;
      p += 4;  // move to the leading slash

      // Step b: Remove all lines that have "(deleted)" in them
      const char *q = strstr(p,"(deleted)");
      if (q)
        continue;

      // Step c: find the filename in the directory
      q = strrchr(p,'/');
      if (q == NULL)
        continue;

      // Step d: Find the .so in the file
      const char * so = strstr(++q,".so");
      if (so == NULL)
        continue;

      // Step e: make sure that any trailing chars are numbers or a period
      so += 3;
      size_t tail = strspn(so,"1234567890.");
      if (0 < tail && tail < 2)
        continue;
      
      // Step f: remove libxalt_init.so
      const char *xalt_so = strstr(q,"libxalt_init.so");
      if (xalt_so)
        continue;

      // Step g: remove trailing newline
      char *nl = strchr(p,'\n');
      if (nl)
        *nl = '\0';

      SET_t* entry;
      HASH_FIND_STR(*libT, p, entry);
      if (!entry)
	{
	  entry = (SET_t *) malloc(sizeof(SET_t));
	  utstring_new(   entry->key);
	  int keylen = strlen(p);
	  utstring_bincpy(entry->key, p, keylen);

	  HASH_ADD_KEYPTR(hh, *libT, utstring_body(entry->key), keylen, entry);
	}
    }
}
