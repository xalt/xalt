#ifndef XALT_UTILS_H
#define XALT_UTILS_H

#include <string>
#include "xalt_types.h"

bool path2module(std::string& path, Table& rmapT, std::string& result);
FILE* xalt_file_open(const char* name);


#endif //XALT_UTILS_H
