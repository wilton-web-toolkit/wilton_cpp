/* 
 * File:   Logger.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:03 PM
 */

#include "wilton/Logger.hpp"

#include <string>

#include "staticlib/config.hpp"
#include "staticlib/icu_utils.hpp"
#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "wilton/wilton.h"

namespace wilton {

namespace { // anonymous

namespace si = staticlib::icu_utils;

} // namespace

class Logger::Impl : public staticlib::pimpl::PimplObject::Impl {
    std::string logger_name;
    
public:
    Impl(const icu::UnicodeString& logger_name) :
    logger_name(si::to_utf8(logger_name)) { }
    
    void debug(Logger&, const icu::UnicodeString& message) {
        log("DEBUG", message);
    }

    void info(Logger&, const icu::UnicodeString& message) {
        log("INFO", message);
    }

    void warn(Logger&, const icu::UnicodeString& message) {
        log("WARN", message);
    }

    void error(Logger&, const icu::UnicodeString& message) {
        log("ERROR", message);
    }
    
private:
    void log(std::string level, const icu::UnicodeString& message) {
        std::string str = si::to_utf8(message);
        char* err = wilton_log(level.c_str(), level.length(), 
                logger_name.c_str(), logger_name.length(), 
                str.c_str(), str.length());
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
    }
    
};
PIMPL_FORWARD_CONSTRUCTOR(Logger, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, void, debug, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, void, info, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, void, warn, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, void, error, (const icu::UnicodeString&), (), WiltonException)

} // namespace
