#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace HtsgetServer {

class ServerConfig {

public:
  std::string BAM_FILE_DIRECTORY = "data/";
  std::string TEMP_FILE_DIRECTORY = "temp/";
  static constexpr int MAX_BYTE_RANGE = 1024;
};

}

#endif