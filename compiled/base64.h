#ifndef BASE64_H
#define BASE64_H

char*          base64_encode(const void* binaryData, int len, int *flen);
unsigned char* base64_decode(const char* ascii,      int len, int *flen);

#endif //BASE64_H
