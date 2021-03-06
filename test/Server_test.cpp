/* 
 * File:   Server_test.cpp
 * Author: alex
 *
 * Created on June 30, 2016, 12:03 PM
 */

#include "wilton/Server.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <vector>

#include <unicode/uclean.h>

#include "staticlib/config/assert.hpp"
#include "staticlib/icu_utils.hpp"
#include "staticlib/httpclient.hpp"
#include "staticlib/serialization.hpp"
#include "staticlib/utils.hpp"

#include "wilton/Logger.hpp"

namespace iu = staticlib::icu_utils;
namespace io = staticlib::io;
namespace sh = staticlib::httpclient;
namespace ss = staticlib::serialization;
namespace su = staticlib::utils;

const uint16_t TCP_PORT = 8080;
const icu::UnicodeString ROOT_URL = "http://127.0.0.1:" + iu::to_ustring(TCP_PORT) + "/";
const uint16_t TCP_PORT_HTTPS = 8443;
const icu::UnicodeString ROOT_URL_HTTPS = "https://localhost:" + iu::to_ustring(TCP_PORT_HTTPS) + "/";
const icu::UnicodeString ROOT_RESP = "Hello C++!\n";
const icu::UnicodeString LOG_DATA = "Please append me to log";
const icu::UnicodeString STATIC_FILE_DATA = "I am data from static file\n";
const icu::UnicodeString STATIC_ZIP_DATA = "I am data from ZIP file\n";

sh::HttpSession HTTP = sh::HttpSession();
std::array<char, 4096> BUFFER;
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
        },
        {
            "/reqfilename",
            [](const wilton::Request& req, wilton::Response& resp) {
                resp.send(req.get_data_filename());
            }
        },
        {
            "/sendfile",
            [](const wilton::Request& /*req*/, wilton::Response& resp) {
                resp.send_file("../test/static/test.txt");
            }
        }
    };
}

icu::UnicodeString http_get(const icu::UnicodeString& url) {
    auto src = HTTP.open_url(iu::to_utf8(url));
    auto sink = iu::ustring_sink();
    io::copy_all(src, sink, BUFFER.data(), BUFFER.size());
    return sink.get_string();
}

uint16_t http_get_code(const icu::UnicodeString& url) {
    auto opts = sh::HttpRequestOptions();
    opts.abort_on_response_error = false;
    auto src = HTTP.open_url(iu::to_utf8(url), opts);
    auto sink = io::null_sink();
    io::copy_all(src, sink, BUFFER.data(), BUFFER.size());
    return src.get_info().response_code;
}

icu::UnicodeString http_get_header(const icu::UnicodeString& url, const icu::UnicodeString& header) {
    auto src = HTTP.open_url(iu::to_utf8(url));
    auto sink = io::null_sink();
    io::copy_all(src, sink, BUFFER.data(), BUFFER.size());
    auto& headers = src.get_info().get_headers();
    return iu::from_utf8(src.get_info().get_header(iu::to_utf8(header)));
}

icu::UnicodeString http_post(const icu::UnicodeString& url, const icu::UnicodeString& data) {
    auto opts = sh::HttpRequestOptions();
    opts.method = "POST";
    opts.force_http_10 = true;
    auto src = HTTP.open_url(iu::to_utf8(url), iu::uarray_source(data), opts);
    auto sink = iu::ustring_sink();
    io::copy_all(src, sink, BUFFER.data(), BUFFER.size());
    return sink.get_string();
}

icu::UnicodeString read_file_to_string(const icu::UnicodeString& filename) {
    auto file = su::FileDescriptor(iu::to_utf8(filename), 'r');
    auto sink = iu::ustring_sink();
    io::copy_all(file, sink, BUFFER.data(), BUFFER.size());
    return sink.get_string();
}

void write_string_to_file(const icu::UnicodeString& filename) {
    auto file = su::FileDescriptor(iu::to_utf8(filename), 'w');
    auto src = iu::uarray_source(filename);
    std::array<char, 1024> buf;
    io::copy_all(src, file, buf.data(), buf.size());
}

void test_simple() {
    ss::JsonValue conf = {
        { "tcpPort", TCP_PORT }
    };
    auto server = wilton::Server(std::move(conf), create_handlers());
    
    slassert(ROOT_RESP == http_get(ROOT_URL));
    slassert("foo" == http_post(ROOT_URL + "postmirror", "foo"));    
    slassert(404 == http_get_code(ROOT_URL + "foo"));
    slassert(STATIC_FILE_DATA == http_get(ROOT_URL + "sendfile"));
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
    // uncomment for testing only this function
//    slassert(LOG_DATA == read_file_to_string(logfile));
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
    ss::JsonValue conf = {
        { "tcpPort", TCP_PORT }
    };
    auto server = wilton::Server(std::move(conf), create_handlers());
    
    slassert(ROOT_RESP == http_get(ROOT_URL));
    auto opts = sh::HttpRequestOptions();
    opts.method = "GET";
    opts.headers.emplace_back("X-Dupl-H", "foo");
    opts.headers.emplace_back("Referer", "foo");
    opts.headers.emplace_back("referer", "bar");
    auto src = HTTP.open_url(iu::to_utf8(ROOT_URL + "headers"), opts);
    auto sink = iu::ustring_sink();
    io::copy_all(src, sink, BUFFER.data(), BUFFER.size());
    
    auto client_headers = ss::load_json_from_ustring(sink.get_string());
    slassert("foo" == client_headers["X-Dupl-H"].as_ustring());
    slassert(ss::JsonType::STRING == client_headers["referer"].type() || 
            ss::JsonType::STRING == client_headers["Referer"].type());
    if (ss::JsonType::STRING == client_headers["referer"].type()) {
        slassert("bar" == client_headers["referer"].as_ustring());
    } else {
        slassert("foo" == client_headers["Referer"].as_ustring());
    }
    slassert("foo" == src.get_info().get_header("X-Server-H1"));
    slassert("bar" == src.get_info().get_header("X-Server-H2"));
    slassert("http" == src.get_info().get_header("X-Proto"));
}

void test_https() {
    auto server = wilton::Server({
        {"tcpPort", TCP_PORT_HTTPS},
        {"ssl",
            {
                {"keyFile", "../test/certificates/server/localhost.pem"},
                {"keyPassword", "test"},
                {"verifyFile", "../test/certificates/server/staticlibs_test_ca.cer"},
                {"verifySubjectSubstr", "CN=testclient"}
            }
        },
    }, create_handlers());
    auto opts = sh::HttpRequestOptions();
    opts.method = "GET";
    opts.sslcert_filename = "../test/certificates/client/testclient.cer";
    opts.sslcertype = "PEM";
    opts.sslkey_filename = "../test/certificates/client/testclient.key";
    opts.ssl_key_type = "PEM";
    opts.ssl_keypasswd = "test";
    opts.ssl_verifyhost = true; // localhost in URL is important here
    opts.ssl_verifypeer = true;
    opts.cainfo_filename = "../test/certificates/client/staticlibs_test_ca.cer";
    auto src = HTTP.open_url(iu::to_utf8(ROOT_URL_HTTPS), opts);
    auto sink = iu::ustring_sink();
    io::copy_all(src, sink, BUFFER.data(), BUFFER.size());
    slassert(ROOT_RESP == sink.get_string());
}

void test_request_data_file() {
    auto server = wilton::Server(ss::load_json_from_ustring(R"( 
{
    "tcpPort": )" + iu::to_ustring(TCP_PORT) + R"(,
    "requestPayload": {
        "tmpDirPath": ".",
        "tmpFilenameLength": 32,
        "memoryLimitBytes": 4
    }
})"), create_handlers());
    
    slassert(ROOT_RESP == http_get(ROOT_URL));
    { // direct writer
        icu::UnicodeString filename = http_post(ROOT_URL + "reqfilename", "foobar");
        slassert(filename.length() >= 34);
        icu::UnicodeString posted = http_post(ROOT_URL + "postmirror", "foobar");
        slassert("foobar" == posted);
    }
    { // on-demand writer
        icu::UnicodeString filename = http_post(ROOT_URL + "reqfilename", "foo");
        slassert(filename.length() >= 34);
        icu::UnicodeString posted = http_post(ROOT_URL + "postmirror", "foobar");
        slassert("foobar" == posted);
    }
}

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

