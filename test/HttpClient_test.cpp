/* 
 * File:   HttpClient_test.cpp
 * Author: alex
 *
 * Created on July 12, 2016, 4:52 PM
 */

#include "wilton/HttpClient.hpp"

#include <iostream>
#include <map>

#include "staticlib/config/assert.hpp"
#include "staticlib/icu_utils.hpp"
#include "staticlib/serialization.hpp"
#include "wilton/HttpServer.hpp"

namespace siu = staticlib::icu_utils;
namespace ss = staticlib::serialization;

void test_execute() {
    auto server = wilton::HttpServer({
        {"numberOfThreads", 1},
        {"tcpPort", 8080},
    }, 
    {
        { "/hello",
            [](const wilton::Request& req, wilton::Response & resp) {
                (void) req;
                resp.send("hi!");
            }
        }
    });
    auto client = wilton::HttpClient();
    icu::UnicodeString res = client.execute("http://127.0.0.1:8080/hello");
    std::cout << siu::to_utf8(res) << std::endl;
    // todo
//    slassert(res == "hi!");
}

void test_send_file() {
    
}

int main() {
    try {
        test_execute();
        test_send_file();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

