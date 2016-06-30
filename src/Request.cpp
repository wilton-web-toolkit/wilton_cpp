/* 
 * File:   Request.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:08 PM
 */

#include "wilton/Request.hpp"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "wilton/wilton.h"


namespace wilton {

namespace { // anonymous

typedef const std::map<icu::UnicodeString, icu::UnicodeString>& headers_map_type;

} // namespace

class Request::Impl : public staticlib::pimpl::PimplObject::Impl {
public:

    const icu::UnicodeString& get_http_version(const Request&) const {
        throw WiltonException("");
    }

    const icu::UnicodeString& get_protocol(const Request&) const {
        throw WiltonException("");
    }

    const icu::UnicodeString& get_method(const Request&) const {
        throw WiltonException("");
    }

    const icu::UnicodeString& get_pathname(const Request&) const {
        throw WiltonException("");
    }

    const icu::UnicodeString& get_quiery(const Request&) const {
        throw WiltonException("");
    }

    const icu::UnicodeString& get_url(const Request&) const {
        throw WiltonException("");
    }

    headers_map_type get_headers(const Request&) const {
        throw WiltonException("");
    }

    icu::UnicodeString get_data(const Request&) const {
        throw WiltonException("");
    }
    
};
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_http_version, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_protocol, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_method, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_pathname, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_quiery, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_url, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, headers_map_type, get_headers, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, icu::UnicodeString, get_data, (), (const), WiltonException)

} // namespace
