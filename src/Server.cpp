/* 
 * File:   Server.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:08 PM
 */

#include "wilton/Server.hpp"

#include <memory>
#include <string>

#include <unicode/locid.h>

#include "staticlib/icu_utils.hpp"
#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "staticlib/serialization.hpp"

#include "wilton/wilton.h"
#include "wilton/Logger.hpp"

namespace wilton {

namespace { // anonymous

namespace iu = staticlib::icu_utils;
namespace ss = staticlib::serialization;

using callbacks_map_type = std::map<icu::UnicodeString, std::function<void(const Request&, Response&)>>;

class ServerDeleter {
public:
    void operator()(wilton_Server* server) {
        wilton_Server_stop(server);
    }
};

class HttpPathDeleter {
public:
    void operator()(wilton_HttpPath* path) {
        wilton_HttpPath_destroy(path);
    }
};

std::unique_ptr<wilton_HttpPath, HttpPathDeleter> create_path(std::string method, std::string path, 
        void* ctx, void (*cb)(void* handler_ctx, wilton_Request* request)) {
    wilton_HttpPath* wp = nullptr;
    auto err = wilton_HttpPath_create(&wp, method.c_str(), method.length(), path.c_str(), path.length(),
            ctx, cb);
    if (nullptr != err) {
        std::string trace = TRACEMSG(err);
        wilton_free(err);
        throw WiltonException(trace);
    }
    return std::unique_ptr<wilton_HttpPath, HttpPathDeleter>(wp, HttpPathDeleter());
}

} // namespace

class Server::Impl : public staticlib::pimpl::PimplObject::Impl {
    Logger logger = Logger("wilton.Server");
    callbacks_map_type callbacks;
    std::unique_ptr<wilton_Server, ServerDeleter> ptr;
    
public:
    Impl(const ss::JsonValue& conf, callbacks_map_type callbacks) :
    callbacks(std::move(callbacks)) { 
//        logger.info("Starting Wilton HTTP Server ...");
        std::string conf_str = ss::dump_json_to_string(conf);
        wilton_Server* srv_ptr;
        // todo: drop this temporary workaround
        std::vector<std::unique_ptr<wilton_HttpPath, HttpPathDeleter>> paths;
        paths.emplace_back(create_path("GET", "/", this, Impl::gateway_cb));
        paths.emplace_back(create_path("POST", "/", this, Impl::gateway_cb));
        paths.emplace_back(create_path("PUT", "/", this, Impl::gateway_cb));
        paths.emplace_back(create_path("DELETE", "/", this, Impl::gateway_cb));
        paths.emplace_back(create_path("OPTIONS", "/", this, Impl::gateway_cb));
        std::vector<wilton_HttpPath*> ptrs;
        for (auto& pt : paths) {
            ptrs.push_back(pt.get());
        }
        // end todo
        char* err = wilton_Server_create(std::addressof(srv_ptr), conf_str.data(), conf_str.length(), 
                ptrs.data(), ptrs.size());
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        this->ptr = std::unique_ptr<wilton_Server, ServerDeleter>(srv_ptr, ServerDeleter());        
        logger.info("Wilton HTTP Server started successfully");
    }
    
private:

    static void gateway_cb(void* gateway_ctx, wilton_Request* request) STATICLIB_NOEXCEPT {
        Logger logger = Logger("wilton.Server.gateway");
        Server::Impl* self = static_cast<Server::Impl*> (gateway_ctx);
        try {
            self->gateway_cb_internal(request);
        } catch (const std::exception& e) {
            logger.error(UTRACEMSG(iu::from_utf8(e.what()) + "\nGateway error"));
        } catch (...) {
            logger.error(UTRACEMSG("Gateway error"));
        }
    }
    
    void gateway_cb_internal(wilton_Request* request) {        
        Request req(request);
        Response resp(request);
        if (logger.is_debug_enabled()) {
            logger.debug(UTRACEMSG("Receiving request, path: [" + req.get_pathname() + "]," +
            " method: [" + req.get_method() + "]"));
        }
        icu::UnicodeString path_str = req.get_pathname();
        iu::ustr_ptr path{path_str};
        path->toLower(icu::Locale::getEnglish());
        auto pa = callbacks.find(path.get());
        if (callbacks.end() != pa) {
            try {
                pa->second(req, resp);
            } catch(const std::exception& e) {
                logger.warn(UTRACEMSG(iu::from_utf8(e.what()) + "\n" + 
                        "Callback error, path: [" + req.get_pathname() + "], " +
                        " method: [" + req.get_method() + "]"));
                resp.set_status_code(500);
                resp.set_status_message("Internal Server Error");
                auto msg = iu::from_utf8(e.what());
                resp.send(UTRACEMSG(msg));
            }
        } else {
            resp.set_status_code(404);
            resp.set_status_message("Not Found");
            resp.send("Path not found: [" + req.get_pathname() + "]");
        }
    }

};
PIMPL_FORWARD_CONSTRUCTOR(Server, (const ss::JsonValue&)(callbacks_map_type), (), WiltonException)

} // namespace
