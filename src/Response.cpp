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

#include "wilton/Logger.hpp"

namespace wilton {

namespace { // anonymous

using headers_map_type = std::map<icu::UnicodeString, icu::UnicodeString>&;
using finalizer_type = std::function<void(bool)>;

namespace iu = staticlib::icu_utils;
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
        this->message = iu::to_utf8(message);
    }

    void set_header(Response&, const icu::UnicodeString& name, const icu::UnicodeString& value) {
        headers.insert(std::make_pair(iu::to_utf8(name), iu::to_utf8(value)));
    }

    void send(Response&, const icu::UnicodeString& data) {
        set_response_metadata();
        std::string data_str = iu::to_utf8(data);
        char* err = wilton_Request_send_response(ptr, data_str.c_str(), data_str.length());
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
    }

    void send_file(Response&, const icu::UnicodeString& filepath, std::function<void(bool)> finalizer) { 
        set_response_metadata();
        std::string filepath_str = iu::to_utf8(filepath);
        char* err = wilton_Request_send_file(ptr, filepath_str.c_str(), filepath_str.length(), 
                static_cast<void*> (new finalizer_type(std::move(finalizer))), finalizer_cb);
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
    }
    
private:
    static void finalizer_cb(void* /* finalizer_type* */ ctx, int success) STATICLIB_NOEXCEPT {
        Logger logger = Logger("wilton.Response.finalizer");
        finalizer_type* fin_ptr = static_cast<finalizer_type*> (ctx);
        finalizer_type fin(std::move(*fin_ptr));
        delete fin_ptr;
        try {
            fin(1 == success);
        } catch (const std::exception& e) {
            logger.error(UTRACEMSG(icu::UnicodeString::fromUTF8(e.what()) + "\nFinalizer error"));
        } catch (...) {
            logger.error(UTRACEMSG("Finalizer error"));
        }
    }
    
    void set_response_metadata() {
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
        char* err = wilton_Request_set_response_metadata(ptr, json_str.c_str(), json_str.length());
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
    }
    
};
PIMPL_FORWARD_CONSTRUCTOR(Response, (void*), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, set_status_code, (uint16_t), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, set_status_message, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, set_header, (const icu::UnicodeString&)(const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, send, (const icu::UnicodeString&), (), WiltonException)
PIMPL_FORWARD_METHOD(Response, void, send_file, (const icu::UnicodeString&)(finalizer_type), (), WiltonException)

} //namespace
