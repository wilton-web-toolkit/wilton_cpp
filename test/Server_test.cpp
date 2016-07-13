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
const icu::UnicodeString STATIC_FILE_DATA = "I am data from static file\n";
const icu::UnicodeString STATIC_ZIP_DATA = "I am data from ZIP file\n";

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
        },
        {
            "/headers",
            [](const wilton::Request& req, wilton::Response& resp) {
                std::vector<ss::JsonField> body;
                for (const auto& pa : req.get_headers()) {
                    body.emplace_back(pa.first, pa.second);
                }
                resp.set_header("X-Server-H1", "foo");
                resp.set_header("X-Server-H2", "bar");
                resp.set_header("X-Proto", "http");
                icu::UnicodeString body_str = ss::dump_json_to_ustring(std::move(body));
                resp.send(body_str);
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

icu::UnicodeString http_get_header(const icu::UnicodeString& url, const icu::UnicodeString& header) {
    auto res = HTTP.execute(url);
    auto pa = res.headers.find(header);
    return res.headers.end() != pa ? pa->second : "";
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

void write_string_to_file(const icu::UnicodeString& filename) {
    auto file = su::FileDescriptor(iu::to_utf8(filename), 'w');
    auto src = iu::uarray_source(filename);
    std::array<char, 1024> buf;
    io::copy_all(src, file, buf.data(), buf.size());
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
    icu::UnicodeString logfile = "Server_test_logging.log";
    // server
    auto server = wilton::Server(ss::load_json_from_ustring(R"(
{
    "tcpPort": )" + iu::to_ustring(TCP_PORT) + R"(,
    "logging": {
        "appenders": [
            {
                "appenderType": "FILE",
                "thresholdLevel": "DEBUG",
                "filePath": ")" + logfile + R"(",
                "layout": "%m"
            }
        ],
        "loggers": [
            {
                "name": "staticlib.httpserver",
                "level": "WARN"
            }, {
                "name": "wilton",
                "level": "WARN"
            }, {
                "name": "test",
                "level": "INFO"
            }
        ]
    }
})"), create_handlers());
    slassert(ROOT_RESP == http_get(ROOT_URL));
    http_post(ROOT_URL + "logger", LOG_DATA);
    slassert(LOG_DATA == read_file_to_string(logfile));
}

void test_document_root() {
    auto server = wilton::Server(ss::load_json_from_ustring(R"( 
{
    "tcpPort": )" + iu::to_ustring(TCP_PORT) + R"(,
    "documentRoots": [
        {
            "resource": "/static/files/",
            "dirPath": "../test/static/",
            "mimeTypes": [
                {
                    "extension": "boo",
                    "mime": "text/x-boo"
                }
            ]
        }, {
            "resource": "/static/",
            "zipPath": "../test/static/test.zip",
            "zipInnerPrefix": "test/"
        }
    ]
})"), create_handlers());
    slassert(ROOT_RESP == http_get(ROOT_URL));
    // deliberated repeated requests
    slassert(STATIC_FILE_DATA == http_get(ROOT_URL + "static/files/test.txt"));
    slassert(STATIC_FILE_DATA == http_get(ROOT_URL + "static/files/test.txt"));
    slassert(STATIC_FILE_DATA == http_get(ROOT_URL + "static/files/test.txt"));
    slassert("text/plain" == http_get_header(ROOT_URL + "static/files/test.txt", "Content-Type"));
    slassert(STATIC_FILE_DATA == http_get(ROOT_URL + "static/files/foo.boo"));
    slassert("text/x-boo" == http_get_header(ROOT_URL + "static/files/foo.boo", "Content-Type"));
    slassert(STATIC_ZIP_DATA == http_get(ROOT_URL + "static/zipped.txt"));
    slassert(STATIC_ZIP_DATA == http_get(ROOT_URL + "static/zipped.txt"));
    slassert(STATIC_ZIP_DATA == http_get(ROOT_URL + "static/zipped.txt"));
}

void test_headers() {
    auto server = wilton::Server({
        {"tcpPort", TCP_PORT}
    }, create_handlers());
    
    slassert(ROOT_RESP == http_get(ROOT_URL));
    auto resp = HTTP.execute(ROOT_URL + "headers", "", {
        {"method", "GET"},
        {"headers", {
                {"X-Dupl-H", "foo"},
                {"Referer", "foo"},
                {"referer", "bar"}
            }
        }
    });
    
    auto client_headers = ss::load_json_from_ustring(resp.data);
    slassert("foo" == client_headers.getu("X-Dupl-H").get_ustring());
    slassert(ss::JsonType::STRING == client_headers.getu("referer").get_type() || 
            ss::JsonType::STRING == client_headers.getu("Referer").get_type());
    if (ss::JsonType::STRING == client_headers.getu("referer").get_type()) {
        slassert("bar" == client_headers.getu("referer").get_ustring());
    } else {
        slassert("foo" == client_headers.getu("Referer").get_ustring());
    }
    auto h1 = resp.headers.find("X-Server-H1");
    slassert(resp.headers.end() != h1 && "foo" == h1->second);
    auto h2 = resp.headers.find("X-Server-H2");
    slassert(resp.headers.end() != h2 && "bar" == h2->second);
    auto xp = resp.headers.find("X-Proto");
    slassert(resp.headers.end() != xp && "http" == xp->second);
}

void test_https() {
    // todo
}

void test_request_data_file() {
    // todo
}

int main() {
    try {
        // should go first for logging init
        test_logging();
        test_simple();
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

