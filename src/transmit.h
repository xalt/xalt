#ifndef TRANSMIT_H
#define TRANSMIT_H
#ifdef __cplusplus
extern "C"
{
#endif

void transmit(const char* transmission, const char* jsonStr, const char* kind, const char* key,
              const char* syshost, char* resultDir, , const char* resultFn);

#ifdef __cplusplus
}
#endif

#endif //TRANSMIT_H
