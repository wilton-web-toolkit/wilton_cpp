/* 
 * File:   Logger_test.cpp
 * Author: alex
 *
 * Created on June 30, 2016, 1:25 PM
 */


#include "wilton/Logger.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

void test_link() {
    wilton::Logger::debug("foo");
    wilton::Logger::info("bar");
    wilton::Logger::warn("baz");
    wilton::Logger::error("42");
}

int main() {
    try {
        test_link();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

