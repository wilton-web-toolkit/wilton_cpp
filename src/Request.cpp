/* 
 * File:   Request.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:08 PM
 */

#include "wilton/Request.hpp"

#include "staticlib/config.hpp"
#include "staticlib/io.hpp"
#include "staticlib/icu_utils.hpp"
#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "staticlib/serialization.hpp"
#include "wilton/wilton.h"

namespace wilton {

namespace { // anonymous

namespace io = staticlib::io;
namespace si = staticlib::icu_utils;
namespace ss = staticlib::serialization;

using headers_map_type = const std::map<icu::UnicodeString, icu::UnicodeString>&;

} // namespace

class Request::Impl : public staticlib::pimpl::PimplObject::Impl {
    wilton_Request* ptr;
    icu::UnicodeString http_version;
    icu::UnicodeString protocol;
    icu::UnicodeString method;
    icu::UnicodeString pathname;
    icu::UnicodeString quiery;
    icu::UnicodeString url;
    std::map<icu::UnicodeString, icu::UnicodeString> headers;
    
public:
    
    ~Impl() STATICLIB_NOEXCEPT { };
    
    Impl(void* ptr) :
    ptr(static_cast<wilton_Request*> (ptr)) {
        char* meta;
        int meta_len;
        char* err = wilton_Request_get_request_metadata(this->ptr, std::addressof(meta), std::addressof(meta_len));
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        ss::JsonValue json;
        try {
            auto src = io::array_source(meta, meta_len);
            json = ss::load_json(src);
            wilton_free(meta);
            meta = nullptr;
        } catch (...) {
            wilton_free(meta);
            throw;
        }
        for (const ss::JsonField& fi : json.get_object()) {
            if ("httpVersion" == fi.get_uname()) {
                this->http_version = fi.get_ustring();
            } else if ("protocol" == fi.get_uname()) {
                this->protocol = fi.get_ustring();
            } else if ("method" == fi.get_uname()) {
                this->method = fi.get_ustring();
            } else if ("pathname" == fi.get_uname()) {
                this->pathname = fi.get_ustring();
            } else if ("quiery" == fi.get_uname()) {
                this->quiery = fi.get_ustring();
            } else if ("url" == fi.get_uname()) {
                this->url = fi.get_ustring();
            } else if ("headers" == fi.get_uname()) {
                for (const ss::JsonField& he : fi.get_object()) {
                    headers.insert(std::make_pair(he.get_uname(), he.get_ustring()));
                }
            }
        }
    }

    const icu::UnicodeString& get_http_version(const Request&) const {
        return http_version;
    }

    const icu::UnicodeString& get_protocol(const Request&) const {
        return protocol;
    }

    const icu::UnicodeString& get_method(const Request&) const {
        return method;
    }

    const icu::UnicodeString& get_pathname(const Request&) const {
        return pathname;
    }

    const icu::UnicodeString& get_quiery(const Request&) const {
        return quiery;
    }

    const icu::UnicodeString& get_url(const Request&) const {
        return url;
    }

    headers_map_type get_headers(const Request&) const {
        return headers;
    }

    icu::UnicodeString get_data(const Request&) const {
        char* data;
        int data_len;
        char* err = wilton_Request_get_request_data(this->ptr, std::addressof(data), std::addressof(data_len));
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        try {
            auto sink = si::ustring_sink();
            io::write_all(sink, data, data_len);
            wilton_free(data);
            data = nullptr;
            return sink.get_string();
        } catch (...) {
            wilton_free(data);
            throw;
        }
    }
    
    icu::UnicodeString get_data_filename(const Request&) const {
        char* filename;
        int filename_len;
        char* err = wilton_Request_get_request_data_filename(this->ptr, std::addressof(filename), 
                std::addressof(filename_len));
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        try {
            auto sink = si::ustring_sink();
            io::write_all(sink, filename, filename_len);
            wilton_free(filename);
            filename = nullptr;
            return sink.get_string();
        } catch (...) {
            wilton_free(filename);
            throw;
        }
    }
};
PIMPL_FORWARD_CONSTRUCTOR(Request, (void*), (), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_http_version, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_protocol, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_method, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_pathname, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_quiery, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, const icu::UnicodeString&, get_url, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, headers_map_type, get_headers, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, icu::UnicodeString, get_data, (), (const), WiltonException)
PIMPL_FORWARD_METHOD(Request, icu::UnicodeString, get_data_filename, (), (const), WiltonException)

} // namespace
