#ifndef TICKET_H
#define TICKET_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Error.h"

class Ticket {

public:
  struct url_type {
    std::string url;
    std::unordered_map<std::string, std::string> headers;
  };

public:
  explicit Ticket(std::string format) : format_(format){};

  json_type to_json() {

    json_type urls;
    for (const auto &url : urls_) {
      urls.push_back(json_type{{"url", url.url}, {"headers", url.headers}});
    }

    json_type j = {
        {"htsget",
         {{"format", format_}, {"urls", urls}, {"sha256", checksum_}}}};

    // json_type j = {{"format", format_}, {"urls", urls}, {"md5", checksum_}};
    return j;
  }

  void checksum(std::string checksum) { checksum_ = checksum; }
  void add_url(url_type url) { urls_.push_back(url); }

private:
  std::string format_;
  std::string checksum_;
  std::vector<url_type> urls_;
};

#endif
