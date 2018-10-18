#include "run_submission.h"
#include <algorithm>
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
  
  if (recordT.size() < 1)
    {
      std::cout << "\nNo XALT Watermark\n" << std::endl;
      return 1;
    }

  std::cout << "****************************************\n"
            << "XALT Watermark: " << exec << "\n"
            << "****************************************\n";
  

  std::vector<std::string> keys;
  keys.reserve(recordT.size());
  for (auto const & it : recordT)
    keys.push_back(it.first);
  std::sort(keys.begin(), keys.end());
  
  for ( auto& it : keys)
    {
      int my_len = leftColWidth - it.length();
      std::cout << it << blanks.substr(1,my_len) << recordT[it] << "\n";
    }

  std::cout << std::endl;

}
