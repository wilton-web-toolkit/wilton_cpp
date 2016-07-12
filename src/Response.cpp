/* 
 * File:   Response.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:08 PM
 */

#include "wilton/Response.hpp"

#include <cstdint>
#include <map>
#include <string>

#include "staticlib/icu_utils.hpp"
#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "staticlib/ranges.hpp"
#include "staticlib/serialization.hpp"
#include "wilton/wilton.h"

namespace wilton {

namespace { // anonymous

using headers_map_type = std::map<icu::UnicodeString, icu::UnicodeString>&;

namespace si = staticlib::icu_utils;
namespace sr = staticlib::ranges;
namespace ss = staticlib::serialization;

} // namespace

class Response::Impl : public staticlib::pimpl::PimplObject::Impl {
    wilton_Request* ptr;
    uint16_t code = 200;
    std::string message = "OK";
    std::map<std::string, std::string> headers;
    
public:
    Impl(void* ptr) :
    ptr(static_cast<wilton_Request*> (ptr)) { }
    
    void set_status_code(Response&, uint16_t code) {
        this->code = code;
    }

    void set_status_message(Response&, const icu::UnicodeString& message) {
        this->message = si::to_utf8(message);
    }

    void set_header(Response&, const icu::UnicodeString& name, const icu::UnicodeString& value) {
        headers.insert(std::make_pair(si::to_utf8(name), si::to_utf8(value)));
    }

    void send(Response&, const icu::UnicodeString& data) {
        // metadata
        auto ra = sr::transform(sr::refwrap(headers), [](std::pair<const std::string, std::string>& pa) {
            return ss::JsonField{pa.first, pa.second};
        });
        auto vec = sr::emplace_to_vector(std::move(ra));
        ss::JsonValue json = {
            {"statusCode", code},
            {"statusMessage", message},
            {"headers", std::move(vec)}
        };
        std::string json_str = ss::dump_json_to_string(json);
        char* err1 = wilton_Request_set_response_metadata(ptr, json_str.c_str(), json_str.length());
        if (nullptr != err1) {
            std::string trace = TRACEMSG(err1);
            wilton_free(err1);
            throw WiltonException(err1);
        }
        
        // data
        std::string data_str = si::to_utf8(data);
        char* err2 = wilton_Request_send_response(ptr, data_str.c_str(), data_str.length());
        if (nullptr != err2) {
            std::string trace = TRACEMSG(err2);
            wilton_free(err2);
            throw WiltonException(err2);
        }
    }
};
PIMPL_FORWARD_CONSTRUCTOR(Response, (void*), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, set_status_code, (uint16_t), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, set_status_message, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, set_header, (const icu::UnicodeString&)(const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, send, (const icu::UnicodeString&), (), WiltonException)

} //namespace
