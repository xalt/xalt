#ifndef CAPTURE_H
#define CAPTURE_H
#include "xalt_types.h"

void replace_all(std::string& s1, const std::string from, const std::string to);
void capture(std::string& cmd, Vstring& result);

#endif //CAPTURE_H
