#ifndef XALT_TYPES_H
#define XALT_TYPES_H

#define HERE fprintf(stderr,"%s:%d\n",__FILE__,__LINE__)

#define DEBUG0(fp,s)          if (xalt_tracing) fprintf(fp,s)
#define DEBUG1(fp,s,x1)       if (xalt_tracing) fprintf(fp,s,(x1))
#define DEBUG2(fp,s,x1,x2)    if (xalt_tracing) fprintf(fp,s,(x1),(x2))
#define DEBUG3(fp,s,x1,x2,x3) if (xalt_tracing) fprintf(fp,s,(x1),(x2),(x3))

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct Libpair
{
  Libpair(std::string libIn, std::string sha1In)
    : lib(libIn), sha1(sha1In) {}

  std::string lib;
  std::string sha1;
};

typedef std::vector<std::string>                    Vstring;
typedef std::unordered_set<std::string>             Set;
typedef std::unordered_map<std::string,std::string> Table;
typedef unsigned int                                uint;

#endif //XALT_TYPES_H
