/* 
 * File:   HttpClient_test.cpp
 * Author: alex
 *
 * Created on July 12, 2016, 4:52 PM
 */

#include "wilton/HttpClient.hpp"

#include <iostream>
#include <map>

#include <unicode/uclean.h>

#include "staticlib/config/assert.hpp"

#include "staticlib/icu_utils.hpp"
#include "staticlib/serialization.hpp"
#include "staticlib/utils.hpp"
#include "wilton/Server.hpp"

namespace iu = staticlib::icu_utils;
namespace io = staticlib::io;
namespace ss = staticlib::serialization;
namespace su = staticlib::utils;

wilton::Server create_server() {
    return wilton::Server({
        {"numberOfThreads", 1},
        {"tcpPort", 8080},
    },
    {
        {
            "/hello",
            [](const wilton::Request& req, wilton::Response & resp) {
                (void) req;
                resp.send("hi!");
            }
        },
        {
            "/mirror",
            [](const wilton::Request& req, wilton::Response & resp) {
                resp.send(req.get_data());
            }
        }
    });
}

void test_execute() {
    auto server = create_server();
    auto client = wilton::HttpClient();
    auto res = client.execute("http://127.0.0.1:8080/hello");
    slassert(res.data == "hi!");
}

void test_send_file() {
    icu::UnicodeString filename = "HttpClient_test_tmp.txt";
    {
        auto tmp_file = su::FileDescriptor(iu::to_utf8(filename), 'w');
        io::write_all(tmp_file, "hello");
    }
    auto server = create_server();
    auto client = wilton::HttpClient();
    // forceHttp10 added to speed up test under valgrind
    auto res = client.send_file("http://127.0.0.1:8080/mirror", filename, {{"forceHttp10", true}});
    slassert(res.data == "hello");
}

int main() {
    try {
        test_execute();
        test_send_file();
        
        u_cleanup();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

