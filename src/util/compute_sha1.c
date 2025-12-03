#include "xalt_config.h"
#include "compute_sha1.h"
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define XALT_SHA1_DIGEST_LENGTH 20

/* left rotate */
#define XALT_SHA1_ROL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

static void xalt_sha1(const unsigned char * data, size_t len, uint8_t digest[XALT_SHA1_DIGEST_LENGTH])
{
  uint32_t  state[5]  = { 0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u, 0xc3d2e1f0u };
  uint8_t * block     = (uint8_t *)data;
  size_t    data_left = len;

  bool done = false;
  while (!done)
    {
      uint8_t tmp_buf[64];

      // Provide 64-byte blocks of data, with padding and bit length at the end
      if (data_left >= 64)
        {
          data_left -= 64;
        }
      else
        {
          if (data_left)
            {
              memcpy(tmp_buf, block, data_left);
              tmp_buf[data_left] = 0x80;
              memset(tmp_buf + data_left + 1, 0, 63 - data_left);
            }
          else
            {
              memset(tmp_buf, 0, 56);
              if (len % 64 == 0)
                {
                  tmp_buf[0] = 0x80;
                }
            }
          if (data_left <= 55)
            {
              int    byte;
              size_t len_bits;
              for (byte = 63, len_bits = len * 8; byte >= 56; byte--, len_bits >>= 8)
                {
                  tmp_buf[byte] = len_bits & 0xff;
                }
              done = true;
            }
          block     = tmp_buf;
          data_left = 0;
        }

      // Process 64-byte block and update cumulative state
      uint32_t w[80] = {
        0,
      };
      for (int i = 0; i < 16; i++)
        {
          int byte;
          int shift;
          for (byte = i * 4, shift = 24; byte <= i * 4 + 3; byte++, shift -= 8)
            {
              w[i] |= ((uint32_t)block[byte]) << shift;
            }
        }
      for (int i = 16; i < 80; ++i)
        {
          w[i] = XALT_SHA1_ROL32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

      uint32_t a = state[0];
      uint32_t b = state[1];
      uint32_t c = state[2];
      uint32_t d = state[3];
      uint32_t e = state[4];

      for (int i = 0; i < 80; ++i)
        {
          uint32_t f;
          uint32_t k;
          uint32_t t;

          if (i < 20)
            {
              f = (b & c) | ((~b) & d);
              k = 0x5a827999u;
            }
          else if (i < 40)
            {
              f = b ^ c ^ d;
              k = 0x6ed9eba1u;
            }
          else if (i < 60)
            {
              f = (b & c) | (b & d) | (c & d);
              k = 0x8f1bbcdcu;
            }
          else
            {
              f = b ^ c ^ d;
              k = 0xca62c1d6u;
            }

          t = XALT_SHA1_ROL32(a, 5) + f + e + k + w[i];
          e = d;
          d = c;
          c = XALT_SHA1_ROL32(b, 30);
          b = a;
          a = t;
        }

      state[0] += a;
      state[1] += b;
      state[2] += c;
      state[3] += d;
      state[4] += e;

      block += 64;
    }

  /* Produce the big-endian digest for the provided data */
  for (int i = 0; i < 5; i++)
    {
      int      byte;
      uint32_t state_i;
      for (byte = i * 4 + 3, state_i = state[i]; byte >= i * 4; byte--, state_i >>= 8)
        {
          digest[byte] = (uint8_t)(state_i & 0xff);
        }
    }
}

int xalt_compute_sha1(const char * fn, char * sha1_str)
{
  sha1_str[0] = '\0';
  int fd      = open(fn, O_RDONLY);
  if (fd < 0)
    {
      perror("xalt_compute_sha1: Error opening the executable");
      return -1;
    }

  struct stat st;
  fstat(fd, &st);
  off_t sz = st.st_size;

  unsigned char hash[XALT_SHA1_DIGEST_LENGTH] = { 0 };
  if (sz)
    {
      void * buf = (unsigned char *)mmap(0, sz, PROT_READ, MAP_SHARED, fd, 0);
      if (buf == MAP_FAILED)
        {
          int save_errno = errno;
          perror("xalt_compute_sha1: Error mmapping the executable");
          close(fd);
          errno = save_errno;
          return -1;
        }
      xalt_sha1(buf, sz, hash);
      int rc = munmap(buf, sz);
      if (rc == -1)
        {
          int save_errno = errno;
          perror("xalt_compute_sha1: Error unmapping the executable");
          close(fd);
          errno = save_errno;
          return -1;
        }
    }
  else
    {
      xalt_sha1(NULL, 0, hash);
    }

  close(fd);

  for (int i = 0; i < XALT_SHA1_DIGEST_LENGTH; i++)
    {
      sprintf(&sha1_str[i * 2], "%02x", hash[i]);
    }

  return 0;
}

#if defined(XALT_SHA1_TEST)
#include <stdlib.h>
int main(int argc, char ** argv)
{
  if (argc < 2)
    {
      fprintf(stderr, "Usage: %s <file...>\n");
      exit(1);
    }

  int           rc = 0;
  unsigned char sha1_hash[XALT_SHA1_DIGEST_LENGTH * 2 + 1];
  for (int i = 1; i < argc; i++)
    {
      if (xalt_compute_sha1(argv[i], sha1_hash) == 0)
        {
          printf("%s  %s\n", sha1_hash, argv[i]);
        }
      else
        {
          fprintf(stderr, "%s: %s: %s\n", argv[0], argv[i], strerror(errno));
          rc = 1;
        }
    }

  exit(rc);
}
#endif
