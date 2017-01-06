#ifndef XALT_UTILS_H
#define XALT_UTILS_H

#include <string>
#include "xalt_types.h"

void  build_xaltDir(std::string& xaltDir, std::string& userName, std::string& baseDir);
void  compute_sha1(std::string& fn, char* sha1);
int   isDirectory(const char *path);
bool  path2module(const char* path, Table& rmapT, char* module_name, int module_name_sz);
FILE* xalt_file_open(const char* name);
FILE* xalt_json_file_open(std::string& rmapD, const char* name);


#endif //XALT_UTILS_H
