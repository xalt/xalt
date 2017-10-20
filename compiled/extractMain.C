#include "run_submission.h"
#include <iostream>

int main(int argc, char* argv[])
{
  if (argc < 2)
    {
      std::cout << "Usage: xalt_extract_record a.out\n";
      exit(1);
    }
        
        
  std::string exec = argv[1];

  Table recordT;
  extractXALTRecord(exec, recordT);

  for ( auto it = recordT.begin(); it != recordT.end(); ++it)
    std::cout << it->first << "\t\t" << it->second << "\n";

  std::cout << std::endl;

}
