#ifndef COMPRESS_STRING_H
#define COMPRESS_STRING_H

#ifdef __cplusplus
extern "C"
{
#endif

int compress_string(const char* in, int compressionlevel, char* out, int* lenOut);

#ifdef __cplusplus
}
#endif
#endif //COMPRESS_STRING_H
