/* 
 * File:   Server_test.cpp
 * Author: alex
 *
 * Created on June 30, 2016, 12:03 PM
 */

#include "wilton/Server.hpp"

#include <iostream>
#include <map>

#include <unicode/uclean.h>

#include "staticlib/config/assert.hpp"
#include "staticlib/icu_utils.hpp"
#include "staticlib/serialization.hpp"

#include "wilton/HttpClient.hpp"

namespace iu = staticlib::icu_utils;
namespace ss = staticlib::serialization;

const uint16_t TCP_PORT = 8080;
const icu::UnicodeString ROOT_URL("http://127.0.0.1:" + iu::to_ustring(TCP_PORT) + "/");
const icu::UnicodeString ROOT_RESP = "Hello C++!\n";

wilton::HttpClient HTTP = wilton::HttpClient();

std::map<icu::UnicodeString, std::function<void(const wilton::Request&, wilton::Response&)>> create_handlers() {
    return
    {
        {
            "/",
            [](const wilton::Request& /* req */, wilton::Response & resp) {
                resp.send(ROOT_RESP);
            }
        },
        {
            "/postmirror",
            [](const wilton::Request& req, wilton::Response & resp) {
                resp.send(req.get_data());
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

void test_simple() {
    auto server = wilton::Server({
        {"tcpPort", TCP_PORT}
    }, create_handlers());
    
    slassert(ROOT_RESP == http_get(ROOT_URL));
    slassert("foo" == http_post(ROOT_URL + "postmirror", "foo"));    
    slassert(404 == http_get_code(ROOT_URL + "foo"));
}

void test_logging() {}
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

