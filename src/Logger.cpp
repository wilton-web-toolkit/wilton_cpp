/* 
 * File:   Logger.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:03 PM
 */

#include "wilton/Logger.hpp"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "wilton/wilton.h"

namespace wilton {

class Logger::Impl : public staticlib::pimpl::PimplObject::Impl {
    
public:
    static void debug(const icu::UnicodeString& message) {
        (void) message;
    }

    static void info(const icu::UnicodeString& message) {
        (void) message;
    }

    static void warn(const icu::UnicodeString& message) {
        (void) message;
    }

    static void error(const icu::UnicodeString& message) {
        (void) message;
    }
    
};
PIMPL_FORWARD_METHOD_STATIC(Logger, void, debug, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD_STATIC(Logger, void, info, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD_STATIC(Logger, void, warn, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD_STATIC(Logger, void, error, (const icu::UnicodeString&), (), WiltonException)

} // namespace
