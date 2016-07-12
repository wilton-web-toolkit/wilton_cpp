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
#include "staticlib/serialization.hpp"
#include "wilton/wilton.h"

namespace wilton {

namespace { // anonymous

namespace si = staticlib::icu_utils;
namespace ss = staticlib::serialization;

} // namespace

class Logger::Impl : public staticlib::pimpl::PimplObject::Impl {
    std::string logger_name;
    
public:
    Impl(const icu::UnicodeString& logger_name) :
    logger_name(si::to_utf8(logger_name)) { }
    
    static void init_logging(const ss::JsonValue& conf) {
        std::string conf_str = ss::dump_json_to_string(conf);
        char* err = wilton_logger_initialize(conf_str.data(), conf_str.length());
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
    }
    
    void debug(Logger&, const icu::UnicodeString& message) {
        log("DEBUG", message);
    }

    bool is_debug_enabled(Logger&) {
        return is_level_enabled("DEBUG");
    }

    void info(Logger&, const icu::UnicodeString& message) {
        log("INFO", message);
    }

    bool is_info_enabled(Logger&) {
        return is_level_enabled("INFO");
    }

    void warn(Logger&, const icu::UnicodeString& message) {
        log("WARN", message);
    }

    bool is_warn_enabled(Logger&) {
        return is_level_enabled("WARN");
    }
    
    void error(Logger&, const icu::UnicodeString& message) {
        log("ERROR", message);
    }

    bool is_error_enabled(Logger&) {
        return is_level_enabled("ERROR");
    }
    
private:
    void log(std::string level, const icu::UnicodeString& message) {
        std::string str = si::to_utf8(message);
        char* err = wilton_logger_log(level.c_str(), level.length(), 
                logger_name.c_str(), logger_name.length(), 
                str.c_str(), str.length());
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
    }
    
    bool is_level_enabled(std::string level) {
        int res = -1;
        char* err = wilton_logger_is_level_enabled(logger_name.c_str(), logger_name.length(),
                level.c_str(), level.length(), std::addressof(res));
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        return 1 == res;
    }
    
};
PIMPL_FORWARD_METHOD_STATIC(Logger, void, init_logging, (const ss::JsonValue&), (), WiltonException)
PIMPL_FORWARD_CONSTRUCTOR(Logger, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, void, debug, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, bool, is_debug_enabled, (), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, void, info, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, bool, is_info_enabled, (), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, void, warn, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, bool, is_warn_enabled, (), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, void, error, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Logger, bool, is_error_enabled, (), (), WiltonException)
        
} // namespace
