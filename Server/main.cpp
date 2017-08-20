#include "json.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING // debugging
// #define NDEBUG

#include "Response.h"
#include "Server.h"
#include "Uri.h"
#include "Utilities.h"

#include "Common.h"
#include <cstdio>
#include <cstdlib>

using namespace asio;
using namespace Http;
using namespace HtsgetServer;
using nlohmann::json;

int main() {

  try {
    io_service io;
    GenericServer::ServerAddr server_address =
        std::make_pair("127.0.0.1", 8888);
    auto app = std::make_unique<GenericServer>(server_address);

    /* url query middleware */
    app->router_.get("/", Handler([](Context &ctx) {
                       ctx.req_.query_ = Uri::make_query(ctx.req_.uri_.query_);

                       json_type urlparse = {
                           {"query", ctx.query_}, {"param", ctx.param_},
                       };
                       std::cout << std::setw(4) << urlparse << std::endl;
                     }));

    /*
        curl --http1.1 -v -X GET
       '127.0.0.1:8888/reads/reads_id?format=BAM&referenceName=chr1&start=10145&end=10150&fields=QNAME,FLAG,POS'
    */
    app->router_.get("/reads/");
    app->router_.get(
        "/reads/<id>", Handler([](Context &ctx) {

          auto format = ctx.query_["format"];
          if (!format.empty() && format != "BAM" && format != "CRAM")
            return send_error(ctx, ResErrorType::UnsupportedFormat,
                              "The requested file format " + format +
                                  " is not supported by the server");

          auto referenceName = ctx.query_["referenceName"];
          auto start = ctx.query_["start"];
          auto end = ctx.query_["end"];

          if (referenceName.empty() && (!start.empty() || !end.empty()))
            return send_error(ctx, ResErrorType::InvalidInput,
                              "Request parameter: start/end specified but "
                              "referenceName unspecified");

          if ((start.empty() && !end.empty()) ||
              (!start.empty() && end.empty()))
            return send_error(
                ctx, ResErrorType::InvalidInput,
                "Request parameter: both start and end must be present/absent");

          unsigned long int startul = 0;
          unsigned long int endul = 0;

          if (!start.empty() && !end.empty()) {
            startul = strtoul(start.c_str(), NULL, 10);
            endul = strtoul(end.c_str(), NULL, 10);
            if (startul > endul)
              return send_error(ctx, ResErrorType::InvalidRange,
                                "Request parameter: start is greater than end");
          }

          std::string command = "samtools view -h ./data/test.bam " +
                                referenceName + ":" + start + "-" + end;

          int bytes_read;
          unsigned char buf[100];
          FILE *fp = popen(command.c_str(), "r");

          if (!fp) {
            std::cout << "h1";
            exit(1);
          }

          while ((bytes_read = std::fread(buf, 1, 100, fp)) > 0) {
            ctx.res_.write_text(std::string(buf, buf + bytes_read));
          }

          if (!pclose(fp)) {
            std::cout << "h2";
            exit(1);
          }

          // ctx.res_.write_json(format_ticket(ctx));

        }));

    std::cout << app->router_ << std::endl;
    app->run();
  } catch (std::exception e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
