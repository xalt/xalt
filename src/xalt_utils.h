#ifndef XALT_UTILS_H
#define XALT_UTILS_H

#include <string>
#include "xalt_types.h"

void  compute_sha1(std::string& fn, char* sha1);
bool  path2module(const char* path, Table& rmapT, char* module_name, int module_name_sz);
FILE* xalt_file_open(const char* name);
FILE* xalt_json_file_open(std::string& rmapD, const char* name);

void build_resultDir(std::string& resultDir, const char* transmission);
void build_resultFn(std::string& resultFn, double start, const char* syshost, const char* uuid, const char *kind,
                    const char* suffix);

#endif //XALT_UTILS_H
