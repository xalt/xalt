#ifndef COMPUTE_SHA1_H
#define COMPUTE_SHA1_H

#include "xalt_c_utils.h"
#include "utstring.h"
#include "xalt_obfuscate.h"

void compute_sha1(const char* fn, char* sha1_str);
void compute_sha1_cleanup();

#endif //COMPUTE_SHA1_H
