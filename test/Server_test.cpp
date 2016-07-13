/* 
 * File:   Server_test.cpp
 * Author: alex
 *
 * Created on June 30, 2016, 12:03 PM
 */

#include "wilton/Server.hpp"

#include <array>
#include <iostream>
#include <map>
#include <vector>

#include <unicode/uclean.h>

#include "staticlib/config/assert.hpp"
#include "staticlib/icu_utils.hpp"
#include "staticlib/serialization.hpp"
#include "staticlib/utils.hpp"

#include "wilton/HttpClient.hpp"
#include "wilton/Logger.hpp"

namespace iu = staticlib::icu_utils;
namespace io = staticlib::io;
namespace ss = staticlib::serialization;
namespace su = staticlib::utils;

const uint16_t TCP_PORT = 8080;
const icu::UnicodeString ROOT_URL = "http://127.0.0.1:" + iu::to_ustring(TCP_PORT) + "/";
const icu::UnicodeString ROOT_RESP = "Hello C++!\n";
const icu::UnicodeString LOG_DATA = "Please append me to log";

wilton::HttpClient HTTP = wilton::HttpClient();
wilton::Logger LOGGER = wilton::Logger("test.wilton.Server_test");

std::map<icu::UnicodeString, std::function<void(const wilton::Request&, wilton::Response&)>> create_handlers() {
    return
    {
        {
            "/",
            [](const wilton::Request& /* req */, wilton::Response& resp) {
                resp.send(ROOT_RESP);
            }
        },
        {
            "/postmirror",
            [](const wilton::Request& req, wilton::Response& resp) {
                resp.send(req.get_data());
            }
        },
        {
            "/logger",
            [](const wilton::Request& req, wilton::Response& /* resp */) {
                LOGGER.info(req.get_data());
            }
        }
    };
}

icu::UnicodeString http_get(const icu::UnicodeString& url) {
    auto res = HTTP.execute(url);
    return res.data;
}

uint16_t http_get_code(const icu::UnicodeString& url) {
    auto res = HTTP.execute(url, "", {
        {"abortOnResponseError", false}
    });
    return res.response_code;
}

icu::UnicodeString http_post(const icu::UnicodeString& url, const icu::UnicodeString& data) {
    auto res = HTTP.execute(url, data, {
        {"method", "POST"}
    });
    return res.data;
}

icu::UnicodeString read_file_to_string(const icu::UnicodeString& filename) {
    auto file = su::FileDescriptor(iu::to_utf8(filename), 'r');
    auto sink = iu::ustring_sink();
    std::array<char, 1024> buf;
    io::copy_all(file, sink, buf.data(), buf.size());
    return sink.get_string();
}

void test_simple() {
    auto server = wilton::Server({
        {"tcpPort", TCP_PORT}
    }, create_handlers());
    
    slassert(ROOT_RESP == http_get(ROOT_URL));
    slassert("foo" == http_post(ROOT_URL + "postmirror", "foo"));    
    slassert(404 == http_get_code(ROOT_URL + "foo"));
}

void test_logging() {
    icu::UnicodeString logfile = "test.log";
    // json literal workaround below for the "list of maps" case
    // appenders    
    std::vector<ss::JsonValue> appenders;
    std::vector<ss::JsonField> ap1;
    ap1.emplace_back("appenderType", "FILE");
    ap1.emplace_back("thresholdLevel", "DEBUG");
    ap1.emplace_back("filePath", logfile);
    ap1.emplace_back("layout", "%m");
    appenders.emplace_back(std::move(ap1));
    // loggers
    std::vector<ss::JsonValue> loggers;
    std::vector<ss::JsonField> lo1;
    lo1.emplace_back("name", "staticlib.httpserver");
    lo1.emplace_back("level", "WARN");
    loggers.emplace_back(std::move(lo1));
    std::vector<ss::JsonField> lo2;
    lo2.emplace_back("name", "wilton");
    lo2.emplace_back("level", "WARN");
    loggers.emplace_back(std::move(lo2));
    std::vector<ss::JsonField> lo3;
    lo3.emplace_back("name", "test");
    lo3.emplace_back("level", "INFO");
    loggers.emplace_back(std::move(lo3));
    // server
    auto server = wilton::Server({
        {"tcpPort", TCP_PORT},
        {"logging",
            {
                {"appenders", std::move(appenders)},
                {"loggers", std::move(loggers)}
            }
        }
    }, create_handlers());
    slassert(ROOT_RESP == http_get(ROOT_URL));
    http_post(ROOT_URL + "logger", LOG_DATA);
    slassert(LOG_DATA == read_file_to_string(logfile));
}

void test_document_root() {}
void test_headers() {}
void test_https() {}
void test_request_data_file() {}


int main() {
    try {
        test_simple();
        test_logging();
        test_document_root();
        test_headers();
        test_https();
        test_request_data_file();
        
        u_cleanup();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

