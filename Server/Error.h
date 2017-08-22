#ifndef COMMON_H
#define COMMON_H

#include "Response.h"
#include "Utilities.h"
#include "json.hpp"

#include <iostream>

using namespace Http;

namespace HtsgetServer {

using json_type = nlohmann::json;

enum class ResErrorType {
  InvalidAuthentication = 0, // Authorization provided is invalid
  PermissionDenied,          // Authorization is required to access the resource
  NotFound,                  // The resource requested was not found
  UnsupportedFormat, // The requested file format is not supported by the server
  InvalidInput, // The request parameters do not adhere to the specification
  InvalidRange, // The requested range cannot be satisfied
};

constexpr static int res_error_code[] = {401, 403, 404, 400, 400, 400};

constexpr static char *res_error_str[] = {(char *)"InvalidAuthentication",
                                          (char *)"PermissionDenied",
                                          (char *)"NotFound",
                                          (char *)"UnsupportedFormat",
                                          (char *)"InvalidInput",
                                          (char *)"InvalidRange"};

constexpr auto errtostr(ResErrorType error) -> char * {
  return enum_map(res_error_str, error);
}
constexpr auto errtoint(ResErrorType error) -> int {
  return enum_map(res_error_code, error);
}
auto res_error(ResErrorType error, std::string message) -> json_type {
  json_type err_msg = {
      {"htsget", {{"error", errtostr(error)}, {"message", message}}}};

  return err_msg;
}

auto send_error(Context &ctx, ResErrorType error, std::string message) -> void {
  ctx.res_.clear_body();
  auto error_res = res_error(error, message);
  ctx.res_.write_json(error_res);
  auto status_code = Response::to_status_code(errtoint(ResErrorType::NotFound));
  ctx.res_.status_code(status_code);
}

}

#endif