#include "run_submission.h"
#include <iostream>

int main(int argc, char* argv[])
{
  const int   leftColWidth = 25;
  std::string blanks       = "                                        ";


  if (argc < 2)
    {
      std::cout << "Usage: xalt_extract_record a.out\n";
      exit(1);
    }
        
        
  std::string exec = argv[1];

  Table recordT;
  extractXALTRecord(exec, recordT);
  


  for ( auto it = recordT.begin(); it != recordT.end(); ++it)
    {
      int my_len = leftColWidth - it->first->length();
      std::cout << it->first << blanks.substr(1,my_len) << it->second << "\n";
    }

  std::cout << std::endl;

}
