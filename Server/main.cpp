#include "json.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING // debugging
// #define NDEBUG

#include "Codec.h"
#include "Response.h"
#include "Server.h"
#include "Uri.h"
#include "Utilities.h"

#include "Common.h"
#include "Config.h"

using namespace asio;
using namespace Http;
using namespace HtsgetServer;
using nlohmann::json;

int main() {

  try {
    auto config = ServerConfig();
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
       '127.0.0.1:8888/reads/test?format=BAM&referenceName=chr1&start=10145&end=10150&fields=QNAME,FLAG,POS'
    */
    app->router_.get("/reads/");
    app->router_.get(
        "/reads/<id>", Handler([&](Context &ctx) {

          auto format = ctx.query_["format"];
          if (!format.empty() && format != "BAM" && format != "CRAM")
            return send_error(ctx, ResErrorType::UnsupportedFormat,
                              "The requested file format " + format +
                                  " is not supported by the server");

          if (format.empty())
            format = "BAM";

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

          /**
           * -#   Outputs requested bam to temporary file,
           * -#   Splits file into chunks
           * -#   Returns a list of tickets, containing urls with byte range to
           * temp file
           */
          std::string command = "samtools view -bh " +
                                config.BAM_FILE_DIRECTORY + ctx.param_["id"] +
                                ".bam " + referenceName + ":" + start + "-" +
                                end;

          std::string bamslicename = SHA256Codec().digest(ctx.param_["id"]);

          std::fstream bamslice;
          bamslice.open(config.TEMP_FILE_DIRECTORY + bamslicename,
                        std::ios::out);

          std::string result;
          Popen proc_pipe{command, "r"};
          while (!(result = proc_pipe.read()).empty()) {
            bamslice << result;
          }

          std::vector<std::string> urls{
              "127.0.0.1:8888/" + config.BAM_FILE_DIRECTORY + bamslicename,
          };

          json_type res = {
              {"format", ctx.query_["format"]},
              {"urls", urls},
              {"md5", "md5_checksum_here"},
          };

          ctx.res_.content_type(
              "application/vnd.ga4gh.htsget.v0.2rc+json; charset=utf-8");
          ctx.res_.write_json(res);

        }));

    /*
        curl --http1.1 -v -X GET -H "Range: bytes=65536-1003750"
       '127.0.0.1:8888/data/test.bam'
    */
    app->router_.get("/data/");
    app->router_.get("/data/<filename>", Handler([](Context &ctx) {
                       std::string range;
                       bool range_exists;
                       std::tie(range, range_exists) =
                           ctx.req_.get_header("Range");

                       int start{};
                       int end{};

                       if (range_exists) {
                         range = range.substr(range.find('=') + 1);
                         start = std::stoi(range.substr(0, range.find('-')));
                         end = std::stoi(range.substr(range.find('-') + 1));
                       }

                       ctx.res_.write_range("./data/" + ctx.param_["filename"] +
                                                std::to_string(start) + "-" +
                                                std::to_string(end),
                                            range, "1000000");
                     }));

    std::cout << app->router_ << std::endl;
    app->run();
  } catch (std::exception e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
