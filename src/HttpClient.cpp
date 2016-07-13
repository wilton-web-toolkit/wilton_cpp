/* 
 * File:   HttpClient.cpp
 * Author: alex
 * 
 * Created on July 12, 2016, 2:23 PM
 */

#include "wilton/HttpClient.hpp"

#include <memory>
#include <string>

#include "staticlib/icu_utils.hpp"
#include "staticlib/io.hpp"
#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "staticlib/serialization.hpp"

#include "wilton/wilton.h"
#include "wilton/Logger.hpp"

namespace wilton {

namespace { // anonymous

namespace ss = staticlib::serialization;
namespace sio = staticlib::io;
namespace siu = staticlib::icu_utils;

using finalizer_type = std::function<void(bool)>;

class ClientDeleter {
public:
    void operator()(wilton_HttpClient* client) {
        wilton_HttpClient_close(client);
    }
};

} // namespace

class HttpClient::Impl : public staticlib::pimpl::PimplObject::Impl {
    std::unique_ptr<wilton_HttpClient, ClientDeleter> ptr;
    
public:    
    Impl(const ss::JsonValue& conf) {
        std::string conf_str = ss::dump_json_to_string(conf);
        wilton_HttpClient* client_ptr;
        char* err = wilton_HttpClient_create(std::addressof(client_ptr), conf_str.data(), conf_str.length());
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        this->ptr = std::unique_ptr<wilton_HttpClient, ClientDeleter>(client_ptr, ClientDeleter());        
    }

    ClientResponse execute(HttpClient&, const icu::UnicodeString& url,
            const icu::UnicodeString& data, const ss::JsonValue& metadata) {
        std::string url_str = siu::to_utf8(url);
        std::string data_str = siu::to_utf8(data);
        std::string metadata_str = ss::dump_json_to_string(metadata);
        char* out;
        int out_len;
        char* err = wilton_HttpClient_execute(ptr.get(), url_str.data(), url_str.length(),
                data_str.data(), data_str.length(), metadata_str.data(), metadata_str.length(),
                std::addressof(out), std::addressof(out_len));
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        auto src = sio::array_source(out, out_len);
        ss::JsonValue json = ss::load_json(src);
        return ClientResponse(std::move(json));
    }

    ClientResponse send_file(HttpClient&, const icu::UnicodeString& url, 
            const icu::UnicodeString& file_path, const ss::JsonValue& metadata,
            finalizer_type finalizer) {
        std::string url_str = siu::to_utf8(url);
        std::string file_path_str = siu::to_utf8(file_path);
        std::string metadata_str = ss::dump_json_to_string(metadata);
        char* out;
        int out_len;
        char* err = wilton_HttpClient_send_file(ptr.get(), url_str.data(), url_str.length(), 
                file_path_str.data(), file_path_str.length(), metadata_str.data(), metadata_str.length(),
                std::addressof(out), std::addressof(out_len),
                static_cast<void*> (new finalizer_type(std::move(finalizer))), finalizer_cb);
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        auto src = sio::array_source(out, out_len);
        ss::JsonValue json = ss::load_json(src);
        return ClientResponse(std::move(json));
    }
    
private:

    static void finalizer_cb(void* /* finalizer_type* */ ctx, int success) STATICLIB_NOEXCEPT {
        Logger logger = Logger("wilton.HttpClient.finalizer");
        finalizer_type* fin_ptr = static_cast<finalizer_type*> (ctx);
        finalizer_type fin(std::move(*fin_ptr));
        delete fin_ptr;
        try {
            fin(1 == success);
        } catch(const std::exception& e) {
            logger.error(UTRACEMSG(icu::UnicodeString::fromUTF8(e.what()) + "\nFinalizer error"));
        } catch(...) {
            logger.error(UTRACEMSG("Finalizer error"));
        }
    }
    
};
PIMPL_FORWARD_CONSTRUCTOR(HttpClient, (const ss::JsonValue&), (), WiltonException)
PIMPL_FORWARD_METHOD(HttpClient, ClientResponse, execute, (const icu::UnicodeString&)
        (const icu::UnicodeString&)(const ss::JsonValue&), (), WiltonException)
PIMPL_FORWARD_METHOD(HttpClient, ClientResponse, send_file, (const icu::UnicodeString&)
        (const icu::UnicodeString&)(const ss::JsonValue&)(finalizer_type), (), WiltonException)        

} // namespace
