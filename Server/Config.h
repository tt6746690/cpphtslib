#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class ServerConfig {

public:
  std::string BAM_FILE_DIRECTORY = "data/";
  std::string TEMP_FILE_DIRECTORY = "temp/";
  int MAX_BYTE_RANGE = 100;
};
#endif