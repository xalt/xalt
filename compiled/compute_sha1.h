#ifndef COMPUTE_SHA1_H
#define COMPUTE_SHA1_H

#include <string>
#include <vector>

struct Arg
{
  Arg(const std::string& fnIn)
    : fn(fnIn), sha1("") {}
  std::string fn;
  std::string sha1;
};

typedef std::vector<Arg> ArgV;

extern ArgV   argV;

void compute_sha1_master(long n);

#endif //COMPUTE_SHA1_H
