/* 
 * File:   Response.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:08 PM
 */

#include "wilton/Response.hpp"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "wilton/wilton.h"

namespace wilton {

namespace { // anonymous

typedef std::map<icu::UnicodeString, icu::UnicodeString>& headers_map_type;

} // namespace

class Response::Impl : public staticlib::pimpl::PimplObject::Impl {
public:
    void set_status_code(Response&, uint16_t code) {
        (void) code;
    }

    void set_status_message(Response&, const icu::UnicodeString& message) {
        (void) message;
    }

    void set_header(Response&, const icu::UnicodeString& name, const icu::UnicodeString& value) {
        (void) name;
        (void) value;
    }

    headers_map_type get_headers(Response&) {
        throw WiltonException("");
    }

    void send(Response&, const icu::UnicodeString& data) {
        (void) data;
    }
};
PIMPL_FORWARD_METHOD(Response, void, set_status_code, (uint16_t), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, set_status_message, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, set_header, (const icu::UnicodeString&)(const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, headers_map_type, get_headers, (), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, send, (const icu::UnicodeString&), (), WiltonException)

} //namespace
