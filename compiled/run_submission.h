#ifndef RUN_SUBMISSION_H
#define RUN_SUBMISSION_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Options.h"

struct Libpair
{
  Libpair(std::string libIn, std::string sha1In)
    : lib(libIn), sha1(sha1In) {}

  std::string lib;
  std::string sha1;
};

typedef std::unordered_map<std::string,std::string> table;

void buildEnvT(char* env[], table& envT);
void buildUserT(Options& options, table& userT);
void capture(std::string& cmd, std::vector<std::string>& result);
void extractXALTRecord(std::string& exec, table& recordT);
void parseLDD(std::string& exec, std::vector<Libpair>& lddA);

extern "C" {
  const char* quotestring(const char* input);
}


#endif //RUN_SUBMISSION_H
