#ifndef WRAPPER_H
#define WRAPPER_H

#include <cstdio>
#include <cstdlib>

#include "Config.h"

namespace HtsgetServer {

class Popen {

public:
  explicit Popen(const char *command, const char *mode) {
    fp_ = popen(command, mode);
  }
  explicit Popen(const std::string &command, const char *mode) {
    fp_ = popen(command.c_str(), mode);
  }
  ~Popen() {
    if (pclose(fp_)) { // returns child PID if processs stopped/terminated
      std::cout << "pclose: fp not closed properly" << std::endl;
      exit(1);
    }
  }

public:
  auto read() -> std::string {
    bytes_read_ = std::fread(buf_, 1, BUF_SIZE, fp_);
    return std::string(buf_, buf_ + bytes_read_);
  }

private:
  static constexpr int BUF_SIZE = 1024 * 1024;
  int bytes_read_;
  unsigned char buf_[BUF_SIZE];
  FILE *fp_;
};
}

#endif